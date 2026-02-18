import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Scanner;

public class Send implements Runnable {
    private final DataOutputStream dOut;
    private final Scanner scanner;
    private final Crypto crypto;
    private volatile boolean running = true;

    public Send(Socket socket, Crypto crypto) throws IOException {
        this.dOut = new DataOutputStream(socket.getOutputStream());
        this.scanner = new Scanner(System.in);
        this.crypto = crypto;
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

                byte[] plaintext = message.getBytes("UTF-8");

                byte[] ciphertext = crypto.encrypt(plaintext);

                dOut.writeInt(ciphertext.length);
                dOut.write(ciphertext);
                dOut.flush();
            }
        }
        catch (IOException e) {
            if (running) System.err.println("Send error: " + e.getMessage());
        }
        catch (Exception e) {
            if (running) System.err.println("Encrypt error: " + e.getMessage());
        }
    }

    public void stop() {
        running = false;
    }
}
