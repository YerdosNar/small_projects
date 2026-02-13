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

    public void punch(String vpsIp, int vpsPort, int listenPort) throws Exception {
        System.out.println("Connecting to rendezvous server...");
        Socket vps = new Socket(vpsIp, vpsPort);

        DataInputStream in = new DataInputStream(vps.getInputStream());
        DataOutputStream out = new DataOutputStream(vps.getOutputStream());

        // Tell server if we can accept connections (0 = no, >0 = port we can listen on)
        out.writeInt(listenPort);
        out.flush();

        System.out.println("Waiting for peer to join...");

        // Read assigned role
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

            // Retry loop
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
            // Both behind NAT - use VPS as relay
            System.out.println("Both peers behind NAT - using relay mode");
            System.out.println("Messages will be relayed through VPS (still encrypted!)");
            
            // Use the VPS connection directly as our socket
            socket = vps;

            doKeyExchange();
            startChat();
        } else {
            vps.close();
            throw new IOException("Unknown role: " + role);
        }
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
        System.out.println("3. Via VPS (NAT traversal)");
        System.out.print("Select: ");
        int choice = sc.nextInt();
        sc.nextLine();

        try {
            if (choice == 1) {
                System.out.print("Peer IP: ");
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
                System.out.print("VPS port (default 8888): ");
                String vpsPortStr = sc.nextLine();
                int vpsPort = vpsPortStr.isEmpty() ? 8888 : Integer.parseInt(vpsPortStr);

                System.out.print("Can you accept connections? Port to listen (0 if behind NAT): ");
                String listenPortStr = sc.nextLine();
                int listenPort = listenPortStr.isEmpty() ? 0 : Integer.parseInt(listenPortStr);

                peer.punch(vpsIp, vpsPort, listenPort);
            }
        }
        catch (Exception e) {
            System.err.println("ERROR: " + e.getMessage());
            e.printStackTrace();
        }

        sc.close();
    }
}
