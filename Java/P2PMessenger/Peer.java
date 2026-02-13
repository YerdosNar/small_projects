import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

public class Peer {
    private Socket socket;
    private Crypto crypto;
    private Send sender;
    private Receive receiver;

    public void connect(String ip, int port) throws Exception {
        System.out.println("Connecting to "+ip+":"+port);
        socket = new Socket(ip, port);
        System.out.println("Connected!");
        doKeyExchange();
        startChat();
    }

    public void listen(int port) throws Exception {
        ServerSocket server = new ServerSocket(port);
        System.out.println("Listening on port: "+port+"...");
        socket = server.accept();
        System.out.println("Peer connected from "+socket.getInetAddress());
        server.close();
        doKeyExchange();
        startChat();
    }

    private void doKeyExchange() throws Exception {
        System.out.println("Performing key exchange...");
        crypto = new Crypto();

        DataInputStream in = new DataInputStream(socket.getInputStream());
        DataOutputStream out = new DataOutputStream(socket.getOutputStream());

        // send our public key
        byte[] ourPubKey = crypto.getPublicKeyBytes();
        out.writeInt(ourPubKey.length);
        out.write(ourPubKey);
        out.flush();

        // receive peer's public key
        int len = in.readInt();
        byte[] peerPubKey = new byte[len];
        in.readFully(peerPubKey);

        // Compute shared secret
        crypto.computeSharedSecret(peerPubKey);
        System.out.println("Encryption established!");
    }

    private void startChat() throws IOException {
        sender = new Send(socket, crypto);
        receiver = new Receive(socket, crypto);

        Thread sendThread = new Thread(sender);
        Thread recvThread = new Thread(receiver);

        recvThread.start();
        sendThread.start();

        try {
            sendThread.join();
        }
        catch(InterruptedException e) {
            System.err.println(e.getMessage());
        }

        receiver.stop();
        socket.close();
        System.out.println("Diconnected...");
    }

    public static void main(String[] args) {
        Peer peer = new Peer();
        Scanner sc = new Scanner(System.in);

        System.out.println("1. Connect to peer");
        System.out.println("2. Wait for peer");
        System.out.print("Select: ");
        int choice = sc.nextInt();
        sc.nextLine();

        try {
            if (choice == 1) {
                System.out.println("Peer IP: ");
                String ip = sc.nextLine();
                System.out.print("Port: ");
                int port = sc.nextInt();
                peer.connect(ip, port);
            }
            else {
                System.out.print("Port to listen: ");
                int port = sc.nextInt();
                peer.listen(port);
            }
        }
        catch (Exception e) {
            System.err.println("ERROR: " + e.getMessage());
            e.printStackTrace();
        }

        sc.close();
    }
}
