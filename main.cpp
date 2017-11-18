#include <iostream>

#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/UdpSocket.h"


#define OUTPUT_BUFFER_SIZE 1024


class ProxyListener : public osc::OscPacketListener {
public:

    ProxyListener(UdpTransmitSocket &proxy) : proxy_(proxy) { ; }

protected:

    virtual void ProcessMessage(const osc::ReceivedMessage &m,
                                const IpEndpointName &remoteEndpoint) {
        (void) remoteEndpoint; // suppress unused parameter warning

        try {
            osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

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

                }
                arg++;
            }


            p << osc::EndMessage;
//            p << osc::EndBundle;


            proxy_.Send(p.Data(), p.Size());
        } catch (osc::Exception &e) {
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
        }


    }

    char buffer[OUTPUT_BUFFER_SIZE];
    UdpTransmitSocket &proxy_;
};

int main(int argc, char *argv[]) {
    (void) argc; // suppress unused parameter warnings
    (void) argv; // suppress unused parameter warnings
    if (argc < 4) {
        std::cerr << "OscProxy listenport proxyhost proxyport" << std::endl;
        return -1;
    }
    unsigned listenport = (unsigned) std::atoi(argv[1]);
    std::string proxyhost = argv[2];
    unsigned proxyport = (unsigned) std::atoi(argv[3]);

    if (listenport == 0) {
        std::cerr << "invalid listenport" << std::endl;
        return -1;
    }

    if (proxyport == 0) {
        std::cerr << "invalid proxyport" << std::endl;
        return -1;
    }
    UdpTransmitSocket proxy(IpEndpointName(proxyhost.c_str(), proxyport));

    ProxyListener listener(proxy);
    UdpListeningReceiveSocket s(
        IpEndpointName(IpEndpointName::ANY_ADDRESS, listenport),
        &listener);

    std::cout << "press ctrl-c to end" << std::endl;

    s.RunUntilSigInt();

    return 0;
}



