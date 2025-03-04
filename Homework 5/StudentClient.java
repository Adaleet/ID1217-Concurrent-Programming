import java.io.*;
import java.net.*;

class StudentClient {
    private static final String SERVER_HOST = "localhost";
    private static final int PORT = 12345;

    public static void main(String[] args) {
        try (Socket socket = new Socket(SERVER_HOST, PORT);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in))) {

            System.out.println("Connected to teacher.");

            out.println("request_partner");   // Send a request for a partner
            System.out.println("Waiting for a partner...");

            String studentInfo = in.readLine();
            if (studentInfo != null) {
                System.out.println(studentInfo);
                Runtime.getRuntime().addShutdownHook(new Thread(() -> System.out.println("[EXIT] " + studentInfo)));
            }
            while (true) {
                String input = userInput.readLine();
                if (input.equalsIgnoreCase("exit")) {
                    System.out.println("Disconnecting...");
                    break;
                }
            }
        } catch (IOException e) {
            System.out.println("Disconnected from teacher.");
        }
    }
}
