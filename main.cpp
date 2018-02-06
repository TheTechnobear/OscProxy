#include <iostream>
#include <memory>

#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"


#define OUTPUT_BUFFER_SIZE 1024


class ProxyListener : public osc::OscPacketListener {
public:

    ProxyListener(const std::string &host, unsigned port)
            : host_(host), port_(port) {
        buffer_[0] = 0;
        if (host.length() > 0) {
            connect(host, port);
        }
    }

protected:
    void connect(const std::string &host, unsigned port) {
        std::cout << "connect " << host << ":" << port << std::endl;
        proxy_ = std::make_shared<UdpTransmitSocket>(IpEndpointName(host.c_str(), port));
    }

    void ProcessMessage(const osc::ReceivedMessage &m,
                        const IpEndpointName &remoteEndpoint) override {
        (void) remoteEndpoint; // suppress unused parameter warning

        try {
            if (!proxy_) {
                char host[128];
                remoteEndpoint.AddressAsString(host);
                if (remoteEndpoint.address == IpEndpointName("localhost").address) {
                    ;
                } else {
                    host_=host;
                    connect(host_,port_);
                }
            }
            if(!proxy_ || !proxy_->IsBound()) return;
            osc::OutboundPacketStream p(buffer_, OUTPUT_BUFFER_SIZE);

//            p << osc::BeginBundleImmediate;
            p << osc::BeginMessage(m.AddressPattern());
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
            while (arg != m.ArgumentsEnd()) {
                switch (arg->TypeTag()) {
                    case osc::INT32_TYPE_TAG : {
                        p << arg->AsInt32();
                        break;
                    };
                    case osc::FLOAT_TYPE_TAG : {
                        p << arg->AsFloat();
                        break;
                    };
                    case osc::STRING_TYPE_TAG : {
                        p << arg->AsString();
                        break;
                    };
                    case osc::MIDI_MESSAGE_TYPE_TAG : {
                        p << osc::MidiMessage(arg->AsMidiMessage());
                        break;
                    };

                    default:
                        break;
                }
                arg++;
            }


            p << osc::EndMessage;
//            p << osc::EndBundle;


            proxy_->Send(p.Data(), p.Size());
        } catch (osc::Exception &e) {
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
        }


    }

    std::shared_ptr<UdpTransmitSocket> proxy_;
    std::string host_;
    unsigned port_;
    char buffer_[OUTPUT_BUFFER_SIZE]{};
};

int main(int argc, char *argv[]) {
    (void) argc; // suppress unused parameter warnings
    (void) argv; // suppress unused parameter warnings
    if (argc < 2) {
        std::cerr << "OscProxy listenport [proxyhost proxyport]" << std::endl;
        return -1;
    }
    char *endp;
    auto listenport = (unsigned) std::strtol(argv[1], &endp, 10);

    std::string proxyhost;
    if (argc > 2) proxyhost = argv[2];

    unsigned proxyport = listenport;
    if (argc > 3) proxyport =  (unsigned) std::strtol(argv[3], &endp, 10);

    if (listenport == 0) {
        std::cerr << "invalid listenport" << std::endl;
        return -1;
    }

    if (proxyport == 0) {
        std::cerr << "invalid proxyport" << std::endl;
        return -1;
    }

    ProxyListener listener(proxyhost, proxyport);

    UdpListeningReceiveSocket s(
            IpEndpointName(IpEndpointName::ANY_ADDRESS, listenport),
            &listener);

    std::cout << "press ctrl-c to end" << std::endl;

    s.RunUntilSigInt();

    return 0;
}



