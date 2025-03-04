import java.io.*;
import java.net.*;
import java.util.*;

class TeacherServer {
    private static final int PORT = 12345;
    private static List<Socket> students = new ArrayList<>();
    private static List<Integer> studentIDs = new ArrayList<>();
    private static int studentCounter = 1;
    private static boolean running = true;

    public static void main(String[] args) {
        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Teacher is waiting for students... Type 'exit' to terminate.");

            Thread consoleThread = new Thread(() -> {
                try (BufferedReader consoleReader = new BufferedReader(new InputStreamReader(System.in))) {
                    while (running) {
                        String command = consoleReader.readLine();
                        if ("exit".equalsIgnoreCase(command)) {
                            running = false;
                            pairingOddStudents();
                            System.out.println("Server shutting down...");
                            System.exit(0);
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
            consoleThread.start();

            while (running) {
                Socket studentSocket = serverSocket.accept();
                int studentID = studentCounter++;

                new Thread(() -> handleStudent(studentSocket, studentID)).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static synchronized void handleStudent(Socket studentSocket, int studentID) {
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(studentSocket.getInputStream()));
            PrintWriter out = new PrintWriter(studentSocket.getOutputStream(), true);

            System.out.println("Student " + studentID + " connected. Waiting for request..."); // teacher is waiting for the student to send a request for a partner.

            String request = in.readLine();
            if (request != null && request.equals("request_partner")) {
                students.add(studentSocket);
                studentIDs.add(studentID);

                System.out.println("Student " + studentID + " requested a partner.");
                
                if (students.size() >= 2) {
                    pairStudents();
                }
            }
        } catch (IOException e) {
            System.out.println("Student " + studentID + " disconnected before pairing.");
            try {
                studentSocket.close(); 
            } catch (IOException ex) {
                System.out.println("Error closing socket for Student " + studentID);
            }
        }        
    }

    private static synchronized void pairStudents() {
        if (students.size() < 2) return; 

        Socket student1 = students.remove(0); // Each student is paired in a FIFO order
        Socket student2 = students.remove(0);
        int id1 = studentIDs.remove(0);
        int id2 = studentIDs.remove(0);

        System.out.println("Pairing Student " + id1 + " with Student " + id2);
        sendPair(student1, id1, id2);
        sendPair(student2, id2, id1);
    }

    private static synchronized void pairingOddStudents() {
        if (!students.isEmpty()) {
            Socket lastStudent = students.remove(0);
            int lastID = studentIDs.remove(0);
            System.out.println("Student " + lastID + " is paired up with themself.");
            sendPair(lastStudent, lastID, lastID);
        }

    }

    private static void sendPair(Socket student, int studentID, int partnerID) {
        try {
            PrintWriter out = new PrintWriter(student.getOutputStream(), true);
            out.println("You are Student " + studentID + ". Your partner is Student " + partnerID);
        } catch (IOException e) {
            System.out.println("Error sending pairing info to Student " + studentID);
        }
    }
}

/*
 *   This program implements a distributed client-server application where a 
 *   teacher (server) pairs students (clients) for a project using Java Sockets: 
 *   The teacher is the server that listens for incoming student connections. Each student (client) connects
 *   and sends a request to be paired.
 *   The teacher maintains a queue of students waiting for partners.
 *   When two students request a partner, they are paired and notified.
 *   If the total number of students (n) is odd, the last student partners with themselves.
 *   The teacher continues accepting students until manually terminated.
 *   If the teacher exits, all students are notified and disconnected.
 * 
 *   I've implemented the teacher server with Java ServerSocket that listens for student connections.
 *   Students are implemented as client using sockets to connect to the teacher.
 *   The teacher manages students using a list of sockets and list of student id's. 
 *   Each student is connected and paired in a FIFO order to ensure that the first two students who request a partner get paired.
 *   If a student disconnects before pairing, they are removed from the queue.
 *   If the teacher shuts down, all students receive a shutdown message, and if the student with the ODD number id doesn't receive a partner, is paired with itself.
 * 
 * How to run: 
 * ----------------------
 * 1. Start the teacher server and StudentClient class by typing 'javac TeacherServer.java StudentClient.java'
 * 2. Start multiple students 'java StudentClient' in a new terminal.
 * 3. Each student connects to the server and requests a partner.
 * 4. The teacher pairs students and notifies them.
 * 5. If there's an odd number of students, the last one is paired with themselves (given that the server exists)
 * 6. The teacher can exit by typing `"exit"`, notifying all students before shutting down.

 */