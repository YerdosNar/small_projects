import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Rendezvous {
    private static final int PORT = 8888;

    public static void main(String[] args) throws Exception {
        ServerSocket server = new ServerSocket(PORT);
        System.out.println("Rendezvous listening on port: "+PORT);

        while(true) {
            System.out.println("\nWaiting for two peers...");

            //Accept first peer
            Socket peer1 = server.accept();
            String addr1 = peer1.getInetAddress().getHostAddress();
            int port1 = peer1.getPort();
            System.out.println("Peer1 connected: "+addr1+":"+port1);

            // Accept second peer
            Socket peer2 = server.accept();
            String addr2 = peer2.getInetAddress().getHostAddress();
            int port2 = peer2.getPort();
            System.out.println("Peer2 connected: "+addr2+":"+port2);

            //Exchange peer info
            DataOutputStream out1 = new DataOutputStream(peer1.getOutputStream());
            DataOutputStream out2 = new DataOutputStream(peer2.getOutputStream());

            // Send peer1 peer2's info and vice versa
            out1.writeUTF(addr2);
            out1.writeInt(port2);
            out1.flush();

            out2.writeUTF(addr1);
            out2.writeInt(port1);
            out2.flush();

            System.out.println("Exchanged peers infos!");

            peer1.close();
            peer2.close();
        }
    }
}
