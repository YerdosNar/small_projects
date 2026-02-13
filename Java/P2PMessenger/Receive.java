import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;

public class Receive implements Runnable {
    private final DataInputStream dIn;
    private final Crypto crypto;
    private volatile boolean running = true;

    public Receive(Socket socket, Crypto crypto) throws IOException {
        this.dIn = new DataInputStream(socket.getInputStream());
        this.crypto = crypto;
    }

    @Override
    public void run() {
        try {
            while(running) {
                // First we get length as in header
                int length = dIn.readInt();


                // Then we get the actual body of data
                byte[] encrypted = new byte[length];
                dIn.readFully(encrypted);

                // Decrypt
                byte[] plaintext = crypto.decrypt(encrypted);
                System.out.println("DEBUG: Cipher: "+encrypted.length+" Plain: "+plaintext.length);

                // Convert it to String and print
                String message = new String(plaintext, "UTF-8");
                System.out.println("\nPeer: " + message);
                System.out.print("You: ");
            }
        }
        catch (IOException e) {
            if (running) { System.err.println("Connection closed"); }
        }
        catch(Exception e) {
            if (running) { System.err.println("Decrypt error"); }
        }
    }

    public void stop() {
        running = false;
    }
}
