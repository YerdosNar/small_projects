import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

public class Peer {
    private Socket socket;
    private Send sender;
    private Receive receiver;

    public void connect(String ip, int port) throws IOException {
        System.out.println("Connecting to "+ip+":"+port);
        socket = new Socket(ip, port);
        System.out.println("Connected!");
        startChat();
    }

    public void listen(int port) throws IOException {
        ServerSocket server = new ServerSocket(port);
        System.out.println("Listening on port: "+port+"...");
        socket = server.accept();
        System.out.println("Peer connected from "+socket.getInetAddress());
        server.close();
        startChat();
    }

    private void startChat() throws IOException {
        sender = new Send(socket);
        receiver = new Receive(socket);

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
        catch (IOException e) {
            System.err.println("ERROR: " + e.getMessage());
        }
    }
}
