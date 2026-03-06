import java.io.*;
import java.util.*;

class Result {

    /*
     * Complete the 'areBracketsProperlyMatched' function below.
     *
     * The function is expected to return a BOOLEAN.
     * The function accepts STRING code_snippet as parameter.
     */

    public static boolean areBracketsProperlyMatched(String code_snippet) {
    // Write your code here
        if (code_snippet == null) return true;
        if (code_snippet.length() == 0
            || code_snippet.isBlank()
            || code_snippet.isEmpty()) return true;

        Stack<Character> stack = new Stack<>();
        for (int i = 0; i < code_snippet.length(); i++) {
            char c = code_snippet.charAt(i);

            if (c == '[' ||
                c == '{' ||
                c == '(')
            {
                stack.push(c);
            }

            if (c == ']' ||
                c == '}' ||
                c == ')')
            {
                if (stack.isEmpty()) return false;
                char p = stack.pop();
                if ((p == '[' && c != ']') ||
                    (p == '{' && c != '}') ||
                    (p == '(' && c != ')'))
                {
                    return false;
                }
            }
        }

        return stack.size() == 0;
    }
}

public class ValidateProperlyNestedBrackets {
    public static void main(String[] args) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(System.in));

        String code_snippet = bufferedReader.readLine();
        boolean result = Result.areBracketsProperlyMatched(code_snippet);
        System.out.println(result ? 1 : 0);
        bufferedReader.close();
    }
}
