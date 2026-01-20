import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class FileWriteByteTest {
    public static void main(String[] args) throws IOException {
        byte[] data = {65, 66, 67, 68, 10};
        Files.write(Path.of("lettersFileWriteByteTest.bin"), data);
        for (byte b:Files.readAllBytes(Path.of("lettersFileWriteByteTest.bin"))) {
            System.out.print((char)b + " ");
        }
    }
}
