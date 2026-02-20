import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Send implements Runnable {
    // Type consts
    public static final byte TYPE_TEXT = 0x01;
    public static final byte TYPE_FILE = 0x02;

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

                if(message.equalsIgnoreCase("/file")) {
                    System.out.print("Enter the filename: ");
                    String filename = scanner.nextLine();
                    // TODO: implement sending a file
                    sendFile(filename);
                    continue;
                }

                sendText(message);
            }
        }
        catch (IOException e) {
            if (running) System.err.println("Send error: " + e.getMessage());
        }
        catch (Exception e) {
            if (running) System.err.println("Encrypt error: " + e.getMessage());
        }
    }

    private void sendText(String message) throws Exception {
        byte[] plaintext = message.getBytes("UTF-8");
        byte[] ciphertext = crypto.encrypt(prependType(TYPE_TEXT, plaintext));

        dOut.writeInt(ciphertext.length);
        dOut.write(ciphertext);
        dOut.flush();
    }

    private void sendFile(String filename) throws Exception {
        Path path = Paths.get(filename);

        if(!Files.exists(path)) {
            System.err.println("File is not found: "+filename);
            return;
        }

        byte[] fileBytes = Files.readAllBytes(path);
        String name = path.getFileName().toString();
        byte[] nameBytes = name.getBytes("UTF-8");

        // Payload: [nameLength(4B)][nameBytes][fileBytes]
        byte[] payload = new byte[4+nameBytes.length+fileBytes.length];
        payload[0] = (byte)(nameBytes.length >> 24);
        payload[1] = (byte)(nameBytes.length >> 16);
        payload[2] = (byte)(nameBytes.length >> 8);
        payload[3] = (byte)(nameBytes.length);
        System.arraycopy(nameBytes, 0, payload, 4, nameBytes.length);
        System.arraycopy(fileBytes, 0, payload, 4+nameBytes.length, fileBytes.length);

        byte[] ciphertext = crypto.encrypt(prependType(TYPE_FILE, payload));

        dOut.writeInt(ciphertext.length);
        dOut.write(ciphertext);
        dOut.flush();

        System.out.println("[Sent file: "+name+" ("+fileBytes.length+" bytes)]");
    }

    private byte[] prependType(byte type, byte[] data) {
        byte[] result = new byte[1+data.length];
        result[0] = type;
        System.arraycopy(data, 0, result, 1, data.length);

        return result;
    }

    public void stop() {
        running = false;
    }
}
