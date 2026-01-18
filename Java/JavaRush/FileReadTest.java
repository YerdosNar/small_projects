import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class FileReadTest {
    public static void readFile(String file) throws IOException {
        FileReader reader = new FileReader(file);
        BufferedReader br = new BufferedReader(reader);
        int i = 1;
        while (br.ready()) {
            System.out.printf("%2d: %s\n", i, br.readLine());
            i++;
        }
        br.close();
    }
    public static void main(String[] args) {
        try {
            readFile("FileReadTest.java");
        }
        catch (IOException e) {
            System.out.println("ERROR: " + e.getMessage());
        }
    }
}
