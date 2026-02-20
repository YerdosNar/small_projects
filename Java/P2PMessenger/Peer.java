import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class Peer {
    private Socket socket;
    private Crypto crypto;
    private Send sender;
    private Receive receiver;
    private final Scanner sc = new Scanner(System.in);

    public void punch(String vpsIp, int vpsPort, int listenPort) throws Exception {
        System.out.println("Connecting to rendezvous server...");
        Socket vps = new Socket(vpsIp, vpsPort);

        DataInputStream in = new DataInputStream(vps.getInputStream());
        DataOutputStream out = new DataOutputStream(vps.getOutputStream());

        out.writeInt(listenPort);
        out.flush();

        System.out.println("Waiting for peer to join...");

        String role = in.readUTF();
        System.out.println("Role assigned: " + role);

        if (role.equals("LISTEN")) {
            int port = in.readInt();
            String peerIp = in.readUTF();
            vps.close();

            System.out.println("Waiting for peer (" + peerIp + ") to connect on port " + port);
            ServerSocket ss = new ServerSocket(port);
            ss.setReuseAddress(true);
            socket = ss.accept();
            ss.close();
            System.out.println("Peer connected!");

            doKeyExchange();
            startChat();

        } else if (role.equals("CONNECT")) {
            String peerIp = in.readUTF();
            int peerPort = in.readInt();
            vps.close();

            System.out.println("Connecting to peer at " + peerIp + ":" + peerPort);

            int attempts = 0;
            while (attempts < 10) {
                try {
                    socket = new Socket(peerIp, peerPort);
                    break;
                } catch (IOException e) {
                    System.out.println("Attempt " + (++attempts) + " failed, retrying...");
                    Thread.sleep(1000);
                }
            }
            if (socket == null) {
                throw new IOException("Failed to connect to peer");
            }
            System.out.println("Connected to peer!");

            doKeyExchange();
            startChat();

        } else if (role.equals("RELAY")) {
            System.out.println("Both peers behind NAT - using relay mode");
            System.out.println("Messages will be relayed through VPS (still encrypted!)");

            socket = vps;

            doKeyExchange();
            startChat();
        } else {
            vps.close();
            throw new IOException("Unknown role: " + role);
        }
    }

    public void connect(String ip, int port) throws Exception {
        System.out.println("Connecting to " + ip + ":" + port);
        socket = new Socket(ip, port);
        System.out.println("Connected!");
        doKeyExchange();
        startChat();
    }

    public void listen(int port) throws Exception {
        ServerSocket server = new ServerSocket(port);
        System.out.println("Listening on port: " + port + "...");
        socket = server.accept();
        System.out.println("Peer connected from " + socket.getInetAddress());
        server.close();
        doKeyExchange();
        startChat();
    }

    private void doKeyExchange() throws Exception {
        System.out.println("Performing key exchange...");
        crypto = new Crypto();

        DataInputStream in = new DataInputStream(socket.getInputStream());
        DataOutputStream out = new DataOutputStream(socket.getOutputStream());

        byte[] ourPubKey = crypto.getPublicKeyBytes();
        out.writeInt(ourPubKey.length);
        out.write(ourPubKey);
        out.flush();

        int len = in.readInt();
        byte[] peerPubKey = new byte[len];
        in.readFully(peerPubKey);

        crypto.computeSharedSecret(peerPubKey);
        System.out.println("Encryption established!");
    }

    private void startChat() throws Exception {
        // Send is constructed first: it asks for the user's name and immediately
        // sends it to the peer over the socket (before any threads are started).
        sender = new Send(socket, crypto, sc);

        // Receive is constructed with the local name so it can use it for prompts.
        receiver = new Receive(socket, crypto, sender.name);

        Thread sendThread = new Thread(sender);
        Thread recvThread = new Thread(receiver);

        recvThread.start();
        sendThread.start();

        try {
            sendThread.join();
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        }

        receiver.stop();
        socket.close();
        System.out.println("Disconnected...");
    }

    public static void main(String[] args) {
        Peer peer = new Peer();

        System.out.println("1. Connect to peer (direct)");
        System.out.println("2. Wait for peer (direct)");
        System.out.println("3. Via VPS (NAT traversal)");
        System.out.print("Select: ");
        int choice = peer.sc.nextInt();
        peer.sc.nextLine();

        try {
            if (choice == 1) {
                System.out.print("Peer IP: ");
                String ip = peer.sc.nextLine();
                System.out.print("Port: ");
                int port = peer.sc.nextInt();
                peer.sc.nextLine();
                peer.connect(ip, port);
            } else if (choice == 2) {
                System.out.print("Port to listen: ");
                int port = peer.sc.nextInt();
                peer.sc.nextLine();
                peer.listen(port);
            } else if (choice == 3) {
                String vpsIp = "";
                System.out.print("IP/Domain name [i/D]: ");
                String ipOrDomain = peer.sc.nextLine();
                if (ipOrDomain.equalsIgnoreCase("i")) {
                    System.out.print("VPS IP: ");
                    vpsIp = peer.sc.nextLine();
                } else {
                    System.out.print("VPS Domain name: ");
                    String domName = peer.sc.nextLine();
                    try {
                        InetAddress inetAddress = InetAddress.getByName(domName);
                        vpsIp = inetAddress.getHostAddress();
                    } catch (UnknownHostException e) {
                        System.err.println("Failed to resolve IP for domain: " + domName);
                        e.printStackTrace();
                    }
                }
                System.out.print("VPS port (default 8888): ");
                String vpsPortStr = peer.sc.nextLine();
                int vpsPort = vpsPortStr.isEmpty() ? 8888 : Integer.parseInt(vpsPortStr);

                System.out.print("Can you accept connections? Port to listen (0 if behind NAT): ");
                String listenPortStr = peer.sc.nextLine();
                int listenPort = listenPortStr.isEmpty() ? 0 : Integer.parseInt(listenPortStr);

                peer.punch(vpsIp, vpsPort, listenPort);
            }
        } catch (Exception e) {
            System.err.println("ERROR: " + e.getMessage());
            e.printStackTrace();
        }

        peer.sc.close();
    }
}
