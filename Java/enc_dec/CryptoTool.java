import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.File;
import java.nio.file.Files;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.util.Arrays;

public class CryptoTool {

    // Algorithm specifications
    private static final String ALGORITHM = "AES";
    private static final String TRANSFORMATION = "AES/CBC/PKCS5Padding";

    public static void main(String[] args) {
        String mode = null;
        String inputFile = null;
        String outputFile = null;
        String password = null;

        // 1. Simple manual argument parsing
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--encrypt":
                    mode = "encrypt";
                    if (i + 1 < args.length) inputFile = args[++i];
                    break;
                case "--decrypt":
                    mode = "decrypt";
                    if (i + 1 < args.length) inputFile = args[++i];
                    break;
                case "-o":
                case "--output":
                    if (i + 1 < args.length) outputFile = args[++i];
                    break;
                case "--key":
                    if (i + 1 < args.length) password = args[++i];
                    break;
            }
        }

        // 2. Validate arguments
        if (mode == null || inputFile == null || outputFile == null || password == null) {
            System.out.println("Usage:");
            System.out.println("  java CryptoTool --encrypt <input> -o <output> --key \"pass\"");
            System.out.println("  java CryptoTool --decrypt <input> -o <output> --key \"pass\"");
            return;
        }

        try {
            File f = new File(inputFile);
            if (!f.exists()) {
                System.err.println("Error: Input file '" + inputFile + "' not found.");
                return;
            }

            // 3. Convert String password to a valid 256-bit AES key using SHA-256
            byte[] keyBytes = MessageDigest.getInstance("SHA-256").digest(password.getBytes("UTF-8"));
            SecretKeySpec secretKey = new SecretKeySpec(keyBytes, ALGORITHM);

            byte[] inputBytes = Files.readAllBytes(f.toPath());
            byte[] outputBytes;

            if ("encrypt".equals(mode)) {
                // Generate a random 16-byte Initialization Vector (IV)
                byte[] iv = new byte[16];
                new SecureRandom().nextBytes(iv);
                IvParameterSpec ivSpec = new IvParameterSpec(iv);

                // Initialize Cipher
                Cipher cipher = Cipher.getInstance(TRANSFORMATION);
                cipher.init(Cipher.ENCRYPT_MODE, secretKey, ivSpec);

                // Encrypt
                byte[] encryptedContent = cipher.doFinal(inputBytes);

                // Prepend IV to the output so we can read it during decryption
                outputBytes = new byte[iv.length + encryptedContent.length];
                System.arraycopy(iv, 0, outputBytes, 0, iv.length);
                System.arraycopy(encryptedContent, 0, outputBytes, iv.length, encryptedContent.length);

                System.out.println("Encryption successful.");

            } else {
                // Read the IV from the first 16 bytes of the file
                if (inputBytes.length < 16) throw new Exception("File too short to be a valid encrypted file.");

                byte[] iv = Arrays.copyOfRange(inputBytes, 0, 16);
                byte[] encryptedContent = Arrays.copyOfRange(inputBytes, 16, inputBytes.length);
                IvParameterSpec ivSpec = new IvParameterSpec(iv);

                // Initialize Cipher
                Cipher cipher = Cipher.getInstance(TRANSFORMATION);
                cipher.init(Cipher.DECRYPT_MODE, secretKey, ivSpec);

                // Decrypt
                outputBytes = cipher.doFinal(encryptedContent);

                System.out.println("Decryption successful.");
            }

            // 4. Write result
            Files.write(new File(outputFile).toPath(), outputBytes);
            System.out.println("Output saved to: " + outputFile);

        } catch (javax.crypto.BadPaddingException e) {
            System.err.println("Error: Decryption failed. Wrong password?");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
