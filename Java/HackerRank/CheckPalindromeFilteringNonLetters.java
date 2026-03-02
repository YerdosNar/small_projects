import java.io.*;

class Result {
    /*
     * Complete the 'isAlphabeticPalindrome' function below.
     *
     * The function is expected to return a BOOLEAN.
     * The function accepts STRING code as parameter.
     */
    public static boolean isAlphabeticPalindrome(String code) {
        // Write your code here
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < code.length(); i++) {
            char c = code.charAt(i);
            int numValue = (int)c;
            if (numValue >= 97 && numValue <= 122) {
                sb.append((char)numValue);
            }
            else if(numValue >= 65 && numValue <= 90) {
                sb.append((char)(numValue+32));
            }
        }

        for (int i = 0; i < sb.length()/2; i++) {
            if (sb.charAt(i) != sb.charAt(sb.length()-1-i)) {
                return false;
            }
        }

        return true;
    }
}

public class CheckPalindromeFilteringNonLetters {
    public static void main(String[] args) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));
        String code = bufferedReader.readLine();
        boolean result = Result.isAlphabeticPalindrome(code);
        System.out.println(result ? 1 : 0);
        bufferedReader.close();
    }
}

