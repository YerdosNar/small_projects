import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Scanner;

public class Send implements Runnable {
    private final DataOutputStream dOut;
    private final Scanner scanner;
    private volatile boolean running = true;

    public Send(Socket socket) throws IOException {
        this.dOut = new DataOutputStream(socket.getOutputStream());
        this.scanner = new Scanner(System.in);
    }

    @Override
    public void run() {
        try {
            while (running) {
                System.out.print("You: ");
                String message = scanner.nextLine();

                if(message.equalsIgnoreCase("/quit")) {
                    running = false;
                    break;
                }

                byte[] data = message.getBytes("UTF-8");
                dOut.writeInt(data.length);
                dOut.write(data);
                dOut.flush();
            }
        }
        catch (IOException e) {
            if (running) System.err.println("Send error: " + e.getMessage());
        }
    }

    public void stop() {
        running = false;
    }
}
