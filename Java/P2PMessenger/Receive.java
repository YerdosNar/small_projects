import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Receive implements Runnable {
    private String peerName = "Peer"; // fallback until name is received
    private final String localName;
    private final DataInputStream dIn;
    private final Crypto crypto;
    private volatile boolean running = true;

    // Directory where received files are saved
    private static final String DOWNLOAD_DIR = "received_files";

    public Receive(Socket socket, Crypto crypto, String localName) throws IOException {
        this.dIn = new DataInputStream(socket.getInputStream());
        this.crypto = crypto;
        this.localName = localName;
        // Ensure download directory exists
        Files.createDirectories(Paths.get(DOWNLOAD_DIR));
    }

    @Override
    public void run() {
        // The very first message from the peer is always their name.
        // Send sends it before its own thread loop starts, so it's safe to block here.
        try {
            int length = dIn.readInt();
            byte[] encrypted = new byte[length];
            dIn.readFully(encrypted);

            byte[] plain = crypto.decrypt(encrypted);
            // Strip the type byte (TYPE_TEXT = 0x01)
            byte[] payload = new byte[plain.length - 1];
            System.arraycopy(plain, 1, payload, 0, payload.length);

            peerName = new String(payload, "UTF-8");
            System.out.println("[" + peerName + " joined the chat]");
            System.out.print(localName + ": ");
        } catch (Exception e) {
            System.err.println("Failed to read peer name: " + e.getMessage());
        }

        try {
            while (running) {
                int length = dIn.readInt();
                byte[] encrypted = new byte[length];
                dIn.readFully(encrypted);

                byte[] plaintext = crypto.decrypt(encrypted);

                byte type = plaintext[0];
                byte[] payload = new byte[plaintext.length - 1];
                System.arraycopy(plaintext, 1, payload, 0, payload.length);

                if (type == Send.TYPE_TEXT) {
                    handleText(payload);
                } else if (type == Send.TYPE_FILE) {
                    handleFile(payload);
                } else {
                    System.err.println("[Unknown message type: " + type + "]");
                }
            }
        } catch (IOException e) {
            if (running) System.err.println("Connection closed");
        } catch (Exception e) {
            if (running) System.err.println("Decrypt error: " + e.getMessage());
        }
    }

    private void handleText(byte[] payload) throws Exception {
        String message = new String(payload, "UTF-8");
        System.out.println("\r" + peerName + ": " + message);
        System.out.print(localName + ": ");
    }

    private void handleFile(byte[] payload) throws Exception {
        // Parse: [nameLength (4 bytes)][nameBytes][fileBytes]
        int nameLength = ((payload[0] & 0xFF) << 24)
                       | ((payload[1] & 0xFF) << 16)
                       | ((payload[2] & 0xFF) << 8)
                       |  (payload[3] & 0xFF);

        byte[] nameBytes = new byte[nameLength];
        System.arraycopy(payload, 4, nameBytes, 0, nameLength);
        String filename = new String(nameBytes, "UTF-8");

        int fileLength = payload.length - 4 - nameLength;
        byte[] fileBytes = new byte[fileLength];
        System.arraycopy(payload, 4 + nameLength, fileBytes, 0, fileLength);

        // Avoid path traversal: use only the base filename
        String safeName = Paths.get(filename).getFileName().toString();
        Path savePath = resolveUnique(Paths.get(DOWNLOAD_DIR, safeName));
        Files.write(savePath, fileBytes);

        System.out.println("\r[" + peerName + " sent a file: " + safeName + " (" + fileLength + " bytes) -> saved to " + savePath + "]");
        System.out.print(localName + ": ");
    }

    /**
     * If the target path already exists, appends (1), (2), etc. to avoid overwriting.
     */
    private Path resolveUnique(Path path) {
        if (!Files.exists(path)) return path;

        String name = path.getFileName().toString();
        Path parent = path.getParent();
        int dot = name.lastIndexOf('.');
        String base = dot >= 0 ? name.substring(0, dot) : name;
        String ext  = dot >= 0 ? name.substring(dot)    : "";

        int counter = 1;
        Path candidate;
        do {
            candidate = parent.resolve(base + " (" + counter++ + ")" + ext);
        } while (Files.exists(candidate));
        return candidate;
    }

    public void stop() {
        running = false;
    }
}
