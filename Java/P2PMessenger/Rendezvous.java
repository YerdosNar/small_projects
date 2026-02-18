import java.io.*;
import java.net.*;

public class Rendezvous {
    private static final int PORT = 8888;

    public static void main(String[] args) throws Exception {
        int port = PORT;
        if (args.length > 0) {
            port = Integer.parseInt(args[0]);
        }

        ServerSocket server = new ServerSocket(port);
        System.out.println("Rendezvous server listening on port: " + port);
        System.out.println("Modes: P2P (direct connection) or RELAY (through this server)");

        while (true) {
            System.out.println("\n========== Waiting for two peers ==========");

            // Accept first peer
            Socket peer1 = server.accept();
            String addr1 = peer1.getInetAddress().getHostAddress();
            System.out.println("Peer 1 connected: " + addr1);

            // Accept second peer
            Socket peer2 = server.accept();
            String addr2 = peer2.getInetAddress().getHostAddress();
            System.out.println("Peer 2 connected: " + addr2);

            DataInputStream in1 = new DataInputStream(peer1.getInputStream());
            DataInputStream in2 = new DataInputStream(peer2.getInputStream());
            DataOutputStream out1 = new DataOutputStream(peer1.getOutputStream());
            DataOutputStream out2 = new DataOutputStream(peer2.getOutputStream());

            // Read if peers can accept connections (have port forwarding)
            int listenPort1 = in1.readInt();  // 0 = can't listen, >0 = can listen on this port
            int listenPort2 = in2.readInt();

            System.out.println("Peer 1 listen port: " + (listenPort1 > 0 ? listenPort1 : "none"));
            System.out.println("Peer 2 listen port: " + (listenPort2 > 0 ? listenPort2 : "none"));

            if (listenPort1 > 0) {
                // Peer1 can listen, peer2 connects to peer1
                out1.writeUTF("LISTEN");
                out1.writeInt(listenPort1);
                out1.writeUTF(addr2);
                out1.flush();

                out2.writeUTF("CONNECT");
                out2.writeUTF(addr1);
                out2.writeInt(listenPort1);
                out2.flush();

                System.out.println("Mode: P2P (Peer2 -> Peer1:" + listenPort1 + ")");
                peer1.close();
                peer2.close();
            } else if (listenPort2 > 0) {
                // Peer2 can listen, peer1 connects to peer2
                out1.writeUTF("CONNECT");
                out1.writeUTF(addr2);
                out1.writeInt(listenPort2);
                out1.flush();

                out2.writeUTF("LISTEN");
                out2.writeInt(listenPort2);
                out2.writeUTF(addr1);
                out2.flush();

                System.out.println("Mode: P2P (Peer1 -> Peer2:" + listenPort2 + ")");
                peer1.close();
                peer2.close();
            } else {
                // Neither can listen - RELAY mode
                out1.writeUTF("RELAY");
                out1.flush();
                out2.writeUTF("RELAY");
                out2.flush();

                System.out.println("Mode: RELAY (both behind NAT)");

                // Start relay threads
                Thread t1 = new Thread(() -> relay(peer1, peer2, "Peer1->Peer2"));
                Thread t2 = new Thread(() -> relay(peer2, peer1, "Peer2->Peer1"));
                t1.start();
                t2.start();
                // Don't close sockets - relay threads own them now
            }
        }
    }

    private static void relay(Socket from, Socket to, String name) {
        try {
            InputStream in = from.getInputStream();
            OutputStream out = to.getOutputStream();
            byte[] buffer = new byte[8192];
            int n;
            while ((n = in.read(buffer)) > 0) {
                out.write(buffer, 0, n);
                out.flush();
            }
        } catch (IOException e) {
            System.out.println(name + " relay ended");
        } finally {
            try { from.close(); } catch (Exception e) {}
            try { to.close(); } catch (Exception e) {}
        }
    }
}
