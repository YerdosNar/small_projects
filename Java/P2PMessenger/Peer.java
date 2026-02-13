import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Scanner;

public class Peer {
    private Socket socket;
    private Crypto crypto;
    private Send sender;
    private Receive receiver;

    public void punch(String vpsIp, int vpsPort, int localPort) throws Exception {
        System.out.println("Connecting to rendezvous server...");
        Socket vps = new Socket(vpsIp, vpsPort);

        DataInputStream in = new DataInputStream(vps.getInputStream());
        String peerIp = in.readUTF();
        int peerPort = in.readInt();
        vps.close();

        System.out.println("Peer is at "+peerIp+":"+peerPort);
        System.out.println("Starting hole punch...");

        // Address reuse enabled
        ServerSocket listenSock = new ServerSocket();
        listenSock.setReuseAddress(true);
        listenSock.bind(new InetSocketAddress(localPort));
        listenSock.setSoTimeout(500);

        socket = null;
        int attempts = 0;
        while (socket == null && attempts < 50) {
            System.out.println("Attempt "+(attempts+1)+"...");

            // Try to accept incoming connection
            try {
                socket = listenSock.accept();
                System.out.println("Accepted connection from peer");
                break;
            }
            catch (SocketTimeoutException e) {
                System.err.println("ERROR: "+e.getMessage());
            }

            try {
                Socket s = new Socket();
                s.setReuseAddress(true);
                s.bind(new InetSocketAddress(localPort));
                s.connect(new InetSocketAddress(peerIp, peerPort), 500);
                socket = s;
                System.out.println("Connected to peer!");
                break;
            }
            catch (IOException e) {
                System.err.println("ERROR: "+e.getMessage());
            }

            attempts++;
            Thread.sleep(300);
        }

        listenSock.close();

        if (socket == null) {
            throw new IOException("Hole punch failed after "+attempts+" attempts");
        }

        System.out.println("Hole punch successful!");
        doKeyExchange();
        startChat();
    }

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

        System.out.println("1. Connect to peer (direct)");
        System.out.println("2. Wait for peer (direct)");
        System.out.println("3. Hole punch (via VPS)");
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
            else if (choice == 2) {
                System.out.print("Port to listen: ");
                int port = sc.nextInt();
                peer.listen(port);
            }
            else if (choice == 3) {
                System.out.print("VPS IP: ");
                String vpsIp = sc.nextLine();
                System.out.print("VPS port (default: 8888): ");
                String vpsPortStr = sc.nextLine();
                int vpsPort = vpsPortStr.isEmpty() ? 8888 : Integer.parseInt(vpsPortStr);
                System.out.print("Local port (default: 9999): ");
                String localPortStr = sc.nextLine();
                int localPort = localPortStr.isEmpty() ? 9999 : Integer.parseInt(localPortStr);

                peer.punch(vpsIp, vpsPort, localPort);
            }
        }
        catch (Exception e) {
            System.err.println("ERROR: " + e.getMessage());
            e.printStackTrace();
        }

        sc.close();
    }
}
