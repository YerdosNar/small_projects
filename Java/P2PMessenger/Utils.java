import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class Utils {
    private static OS getOS() {
        String osName = System.getProperty("os.name").toLowerCase();
        if (osName.contains("win")) {
            return OS.WINDOWS;
        } else if (osName.contains("nix") || osName.contains("nux") || osName.contains("mac")) {
            return OS.UNIX; // Covers Linux, macOS, BSD
        } else {
            return OS.UNKNOWN;
        }
    }

    enum OS { WINDOWS, UNIX, UNKNOWN }

    private static int getUnixConsoleWidth() throws IOException {
        Process process = new ProcessBuilder("tput", "cols")
        .redirectErrorStream(true) // Merge error stream to input stream
        .start();

        try (BufferedReader reader = new BufferedReader(
            new InputStreamReader(process.getInputStream()))) {
            String output = reader.readLine();
            if (output == null || output.trim().isEmpty()) {
                return -1; // Command failed
            }
            return Integer.parseInt(output.trim());
        } finally {
            process.destroy();
        }
    }

    private static int getWindowsConsoleWidth() throws IOException {
        Process process = new ProcessBuilder("cmd", "/c", "mode con")
        .redirectErrorStream(true)
        .start();

        try (BufferedReader reader = new BufferedReader(
            new InputStreamReader(process.getInputStream(), StandardCharsets.UTF_8))) {
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.trim().startsWith("Columns:")) {
                    String[] parts = line.split(":", 2);
                    return Integer.parseInt(parts[1].trim());
                }
            }
            return -1; // Columns not found
        } finally {
            process.destroy();
        }
    }

    public static int detectConsoleWidth() {
        OS os = getOS();
        try {
            switch (os) {
                case WINDOWS:
                return getWindowsConsoleWidth();
                case UNIX:
                return getUnixConsoleWidth();
                default:
                return -1; // Unknown OS
            }
        } catch (IOException | NumberFormatException e) {
            return -1; // Command failed or parsing error
        }
    }

    public static void drawMessageBubble(String str) {
        int width = detectConsoleWidth();

        System.out.println(width);
    }

    public static void main(String[] args) {
        drawMessageBubble("he");
    }
}
