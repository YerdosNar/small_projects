import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Scanner;

public class Send implements Runnable {
    // Message type constants
    public static final byte TYPE_TEXT = 0x01;
    public static final byte TYPE_FILE_META = 0x02;
    public static final byte TYPE_FILE_CHNK = 0x03;

    public final String name;

    private final DataOutputStream dOut;
    private final Scanner scanner;
    private final Crypto crypto;
    private volatile boolean running = true;

    public Send(Socket socket, Crypto crypto, Scanner scanner) throws Exception {
        this.dOut = new DataOutputStream(socket.getOutputStream());
        this.scanner = scanner;
        this.crypto = crypto;

        System.out.print("Your name: ");
        this.name = scanner.nextLine().trim();

        // Send our name to the peer immediately, before threads start,
        // so Receive can reliably read it at the start of its run().
        System.out.println("Sending your name to the peer...");
        sendText(name);
    }

    @Override
    public void run() {
        try {
            while (running) {
                System.out.print(name + ": ");
                String message = scanner.nextLine();

                if (message.equalsIgnoreCase("/quit")) {
                    running = false;
                    break;
                }

                if (message.equalsIgnoreCase("/file")) {
                    System.out.print("Enter the filename: ");
                    String filename = scanner.nextLine().trim();
                    sendFile(filename);
                    continue;
                }

                sendText(message);
            }
        } catch (IOException e) {
            if (running) System.err.println("Send error: " + e.getMessage());
        } catch (Exception e) {
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

        if (!Files.exists(path)) {
            System.err.println("File not found: " + filename);
            return;
        }

        long fileSize = Files.size(path);
        String fname = path.getFileName().toString();
        byte[] nameBytes = fname.getBytes("UTF-8");

        // Payload: [nameLength (4 bytes)][nameBytes][fileBytes]
        ByteBuffer payload = ByteBuffer.allocate(4 + nameBytes.length + 8);
        payload.putInt(nameBytes.length);
        payload.put(nameBytes);
        payload.putLong(fileSize);

        byte[] ciphertext = crypto.encrypt(prependType(TYPE_FILE_META, payload.array()));
        dOut.writeInt(ciphertext.length);
        dOut.write(ciphertext);
        dOut.flush();

        // Stream file chunks
        int chunkSize = 1024 * 1024; // 1MB
        byte[] buffer = new byte[chunkSize];
        long bytesSent = 0;

        System.out.println("Uploading " + fname + "...");

        try (InputStream fis = Files.newInputStream(path)) {
            int read;
            while((read = fis.read(buffer)) > 0) {
                // Encrypt only read bytes
                byte[] chunkPayload;
                if(read == chunkSize) {
                    chunkPayload = buffer;
                }
                else {
                    chunkPayload = new byte[read];
                    System.arraycopy(buffer, 0, chunkPayload, 0, read);
                }

                byte[] chunkCipher = crypto.encrypt(prependType(TYPE_FILE_CHNK, chunkPayload));
                dOut.writeInt(chunkCipher.length);
                dOut.write(chunkCipher);

                bytesSent += read;
                Utils.printProgressBar(bytesSent, fileSize);
            }
        }

        dOut.flush();
        System.out.println("[Sent file: " + fname + " (" + fileSize + " bytes)]");
    }

    private byte[] prependType(byte type, byte[] data) {
        byte[] result = new byte[1 + data.length];
        result[0] = type;
        System.arraycopy(data, 0, result, 1, data.length);
        return result;
    }

    public void stop() {
        running = false;
    }
}
