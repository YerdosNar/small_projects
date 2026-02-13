import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;

public class Receive implements Runnable {
    private final DataInputStream dIn;
    private volatile boolean running = true;

    public Receive(Socket socket) throws IOException {
        this.dIn = new DataInputStream(socket.getInputStream());
    }

    @Override
    public void run() {
        try {
            while(running) {
                // First we get length as in header
                int length = dIn.readInt();

                // Then we get the actual body of data
                byte[] data = new byte[length];
                dIn.readFully(data);

                // Convert it to String and print
                String message = new String(data, "UTF-8");
                System.out.println("\nPeer: " + message);
                System.out.print("You: ");
            }
        }
        catch (IOException e) {
            if (running) { System.err.println("Connection closed"); }
        }
    }

    public void stop() {
        running = false;
    }
}
