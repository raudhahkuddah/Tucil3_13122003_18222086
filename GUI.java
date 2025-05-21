import java.awt.*;
import java.awt.Color;
import java.awt.event.*;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import javax.imageio.ImageIO;
import javax.swing.*;

public class GUI {

    private int height, width;
    private char[][] gridData;
    private JPanel gridPanel;
    private Character currentChar = null;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new GUI().createAndShowGUI());
    }

    private void createAndShowGUI() {
        JFrame frame = new JFrame("Rush Hour Puzzle Generator");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(700, 700);
        frame.setLayout(new BorderLayout());

        // Top input panel
        JPanel inputPanel = new JPanel(new FlowLayout());
        JLabel heightLabel = new JLabel("Height:");
        JTextField heightField = new JTextField(5);
        JLabel widthLabel = new JLabel("Width:");
        JTextField widthField = new JTextField(5);
        JButton renderButton = new JButton("Render");
        JButton putPieceButton = new JButton("Put Piece");
        JButton saveButton = new JButton("Save");
        JButton solveButton = new JButton("Solve");

        inputPanel.add(heightLabel);
        inputPanel.add(heightField);
        inputPanel.add(widthLabel);
        inputPanel.add(widthField);
        inputPanel.add(renderButton);
        inputPanel.add(putPieceButton);
        inputPanel.add(saveButton);
        inputPanel.add(solveButton);

        gridPanel = new JPanel();
        JScrollPane scrollPane = new JScrollPane(gridPanel);
        frame.add(inputPanel, BorderLayout.NORTH);
        frame.add(scrollPane, BorderLayout.CENTER);

        renderButton.addActionListener(_ -> {
            try {
                height = Integer.parseInt(heightField.getText());
                width = Integer.parseInt(widthField.getText());
                if (height <= 0 || width <= 0)
                    throw new NumberFormatException();

                int fullHeight = height + 2;
                int fullWidth = width + 2;

                gridData = new char[fullHeight][fullWidth];
                gridPanel.removeAll();
                gridPanel.setLayout(new GridLayout(fullHeight, fullWidth, 2, 2));

                for (int row = 0; row < fullHeight; row++) {
                    for (int col = 0; col < fullWidth; col++) {
                        JPanel cell = new JPanel();
                        boolean isOuter = (row == height + 1 || col == width + 1 || row == 0 || col == 0);
                        cell.setBackground(isOuter ? Color.DARK_GRAY : Color.LIGHT_GRAY);
                        cell.setBorder(BorderFactory.createLineBorder(Color.BLACK));

                        int finalRow = row;
                        int finalCol = col;

                        cell.addMouseListener(new MouseAdapter() {
                            @Override
                            public void mouseClicked(MouseEvent e) {
                                if (currentChar != null) {
                                    gridData[finalRow][finalCol] = currentChar;
                                    cell.setBackground(getColorForChar(currentChar));
                                    cell.removeAll();
                                    cell.add(new JLabel(currentChar.toString()));
                                    cell.revalidate();
                                    cell.repaint();
                                }
                            }
                        });

                        gridPanel.add(cell);
                    }
                }
                putPieceButton.setVisible(true);
                gridPanel.revalidate();
                gridPanel.repaint();

            } catch (NumberFormatException ex) {
                JOptionPane.showMessageDialog(frame, "Please enter valid positive integers for height and width.");
            }
        });

        putPieceButton.addActionListener(_ -> {
            if (gridData == null) {
                JOptionPane.showMessageDialog(null, "No grid. Please render first.");
                return;
            }
            String input = JOptionPane.showInputDialog(frame, "Enter character A-Z for the piece:");
            if (input != null && input.length() == 1) {
                char ch = Character.toUpperCase(input.charAt(0));
                if (ch >= 'A' && ch <= 'Z') {
                    currentChar = ch;
                } else {
                    JOptionPane.showMessageDialog(frame, "Please enter a letter A-Z.");
                }
            }
        });

        saveButton.addActionListener(_ -> {
            if (gridData == null) {
                JOptionPane.showMessageDialog(null, "No grid to save. Please render and add pieces first.");
                return;
            }

            // Predetermined directory
            java.io.File dir = new java.io.File("test");
            if (!dir.exists()) {
                dir.mkdirs(); // Create directory if it doesn't exist
            }

            while (true) {
                String folderName = JOptionPane.showInputDialog(null, "Enter testcase name (folder):");
                if (folderName == null || folderName.trim().isEmpty()) {
                    return;
                }

                File testcaseDir = new File(dir, folderName);
                File problemFile = new File(testcaseDir, "problem.txt");

                if (testcaseDir.exists()) {
                    int choice = JOptionPane.showConfirmDialog(null,
                            "Testcase name already taken. Overwrite?",
                            "Testcase name is used!",
                            JOptionPane.YES_NO_OPTION);

                    if (choice != JOptionPane.YES_OPTION) {
                        continue; // Reprompt
                    }
                }

                // Create or overwrite directory
                testcaseDir.mkdirs();

                try (java.io.PrintWriter writer = new java.io.PrintWriter(problemFile)) {
                    // 1. Write height and width
                    writer.println(height + " " + width);

                    String exitPosition = null;
                    // 2. Count distinct characters
                    java.util.Set<Character> pieces = new java.util.HashSet<>();
                    for (int i = 0; i <= height + 1; i++) {
                        for (int j = 0; j <= width + 1; j++) {
                            char c = gridData[i][j];
                            if (c == 'K') {
                                if (i == 0)
                                    exitPosition = "UP";
                                if (j == 0)
                                    exitPosition = "LEFT";
                                if (j == width + 1)
                                    exitPosition = "RIGHT";
                                if (i == height + 1)
                                    exitPosition = "DOWN";
                            } else if (c != '\0' && c != 'P') {
                                pieces.add(c);
                            }
                        }
                    }
                    writer.println(pieces.size());
                    // 3. Write grid rows
                    if (exitPosition == null)
                        throw new Exception();
                    if (exitPosition == "UP") {
                        // print top-most layer
                        for (int j = 1; j <= width; j++) {
                            char c = gridData[0][j];
                            writer.print((c == '\0' ? ' ' : c));
                        }
                        writer.println();
                        for (int i = 1; i <= height; i++) {
                            for (int j = 1; j <= width; j++) {
                                char c = gridData[i][j];
                                writer.print((c == '\0' ? '.' : c));
                            }
                            if (i < height)
                                writer.println();
                        }
                    } else if (exitPosition == "DOWN") {
                        for (int i = 1; i <= height; i++) {
                            for (int j = 1; j <= width; j++) {
                                char c = gridData[i][j];
                                writer.print((c == '\0' ? '.' : c));
                            }
                            writer.println();
                        }
                        // print bottom-most layer
                        for (int j = 1; j <= width; j++) {
                            char c = gridData[height + 1][j];
                            writer.print((c == '\0' ? ' ' : c));
                        }
                    } else if (exitPosition == "RIGHT") {
                        // print all the way to the right
                        for (int i = 1; i <= height; i++) {
                            for (int j = 1; j <= width; j++) {
                                char c = gridData[i][j];
                                writer.print((c == '\0' ? '.' : c));
                            }
                            if (gridData[i][width + 1] == 'K')
                                writer.print('K');
                            if (i < height)
                                writer.println();
                        }
                    } else {
                        // print all the way to the left
                        for (int i = 1; i <= height; i++) {
                            for (int j = 0; j <= width; j++) {
                                if (j > 0) {
                                    char c = gridData[i][j];
                                    writer.print((c == '\0' ? '.' : c));
                                } else
                                    writer.print((gridData[i][j] == 'K') ? "K" : ' ');
                            }
                            if (i < height)
                                writer.println();
                        }
                    }

                    JOptionPane.showMessageDialog(null, "Puzzle saved to " + problemFile.getAbsolutePath());
                    putPieceButton.setVisible(false);
                    solveButton.setVisible(true);
                } catch (Exception ex) {
                    JOptionPane.showMessageDialog(null, "Error saving file: " + ex.getMessage());
                }

                break; // Exit loop once saved
            }
        });

        solveButton.addActionListener(_ -> {
            // 1. Prompt for directory
            JFileChooser chooser = new JFileChooser();
            chooser.setDialogTitle("Select Problem Folder");
            chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            chooser.setAcceptAllFileFilterUsed(false);

            int result = chooser.showOpenDialog(null);
            if (result != JFileChooser.APPROVE_OPTION) {
                return; // Cancelled
            }

            File selectedFolder = chooser.getSelectedFile();

            // 2. Prompt for solving method
            String[] methods = { "UCS", "A*", "GBFS", "IDA*" };
            String method = (String) JOptionPane.showInputDialog(
                    null,
                    "Choose solving method:",
                    "Solve Puzzle",
                    JOptionPane.PLAIN_MESSAGE,
                    null,
                    methods,
                    methods[0]);

            if (method == null)
                return; // Cancelled

            String heuristic = null;

            // 3. Prompt for heuristic if not UCS
            if (!method.equals("UCS")) {
                String[] heuristics = { "DUMBASS", "LAZY" };
                heuristic = (String) JOptionPane.showInputDialog(
                        null,
                        "Choose heuristic:",
                        "Select Heuristic",
                        JOptionPane.PLAIN_MESSAGE,
                        null,
                        heuristics,
                        heuristics[0]);

                if (heuristic == null)
                    return; // Cancelled
            }

            // 4. Construct command
            File problemFile = selectedFolder;

            List<String> command = new ArrayList<>();
            command.add("./bin/main.exe");
            command.add(problemFile.getAbsolutePath());
            command.add(method);
            if (heuristic != null) {
                command.add(heuristic);
            }

            try {
                ProcessBuilder pb = new ProcessBuilder(command);
                pb.redirectErrorStream(true);
                Process process = pb.start();

                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                StringBuilder output = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    output.append(line).append("\n");
                }

                process.waitFor();

                // 5. Read solutions.txt
                File solutionsFile = new File(problemFile, "solutions.txt");
                if (!solutionsFile.exists()) {
                    JOptionPane.showMessageDialog(null, "solutions.txt not found.");
                    return;
                }

                List<char[][]> steps = new ArrayList<>();
                int runtime, visited, numSteps, h, w;

                try (BufferedReader solReader = new BufferedReader(new java.io.FileReader(solutionsFile))) {
                    runtime = Integer.parseInt(solReader.readLine().trim());
                    visited = Integer.parseInt(solReader.readLine().trim());
                    numSteps = Integer.parseInt(solReader.readLine().trim());

                    String[] dims = solReader.readLine().trim().split("\\s+");
                    h = Integer.parseInt(dims[0]);
                    w = Integer.parseInt(dims[1]);

                    for (int s = 0; s < numSteps; s++) {
                        char[][] board = new char[h][w];
                        for (int i = 0; i < h; i++) {
                            String row = solReader.readLine();
                            for (int j = 0; j < w; j++) {
                                board[i][j] = row.charAt(j);
                            }
                        }
                        steps.add(board);
                    }
                }

                // 6. Save start.png and end.png
                BufferedImage startImg = renderBoard(steps.get(0), w, h);
                BufferedImage endImg = renderBoard(steps.get(steps.size() - 1), w, h);

                ImageIO.write(startImg, "png", new File(problemFile, "start.png"));
                ImageIO.write(endImg, "png", new File(problemFile, "end.png"));

                // 7. Show metadata popup
                JOptionPane.showMessageDialog(null,
                        "Runtime: " + runtime + " ms\n" +
                                "Visited Nodes: " + visited + "\n" +
                                "Number of Steps: " + numSteps);

                // 8. Show solution viewer
                JFrame solutionFrame = new JFrame("Solution Viewer");
                solutionFrame.setSize(600, 600);
                solutionFrame.setLayout(new BorderLayout());

                JPanel boardPanel = new JPanel();
                boardPanel.setLayout(new GridLayout(h, w, 2, 2));
                solutionFrame.add(boardPanel, BorderLayout.CENTER);

                JButton prevButton = new JButton("Prev");
                JButton nextButton = new JButton("Next");
                JLabel stepLabel = new JLabel();

                JPanel navPanel = new JPanel();
                navPanel.add(prevButton);
                navPanel.add(stepLabel);
                navPanel.add(nextButton);
                solutionFrame.add(navPanel, BorderLayout.SOUTH);

                final int[] stepIndex = { 0 };

                Runnable renderStep = () -> {
                    boardPanel.removeAll();
                    char[][] board = steps.get(stepIndex[0]);

                    for (int i = 0; i < h; i++) {
                        for (int j = 0; j < w; j++) {
                            JPanel cell = new JPanel();
                            cell.setPreferredSize(new Dimension(40, 40));
                            cell.setBorder(BorderFactory.createLineBorder(Color.BLACK));
                            char c = board[i][j];
                            cell.setBackground(c == '.' ? Color.white : getColorForChar(c));
                            if (c != '.')
                                cell.add(new JLabel(Character.toString(c)));
                            boardPanel.add(cell);
                        }
                    }

                    stepLabel.setText("STEP " + (stepIndex[0] + 1) + " / " + steps.size());

                    boardPanel.revalidate();
                    boardPanel.repaint();
                };

                renderStep.run();

                prevButton.addActionListener(_2 -> {
                    if (stepIndex[0] > 0) {
                        stepIndex[0]--;
                        renderStep.run();
                    }
                });

                nextButton.addActionListener(_2 -> {
                    if (stepIndex[0] < steps.size() - 1) {
                        stepIndex[0]++;
                        renderStep.run();
                    }
                });

                solutionFrame.setVisible(true);

            } catch (Exception ex) {
                ex.printStackTrace();
                JOptionPane.showMessageDialog(null,
                        "Failed to run solver or read output: " + ex.getMessage());
            }
        });

        frame.setVisible(true);
    }

    private static final Map<Character, Color> fixedColorMap = Map.ofEntries(
            Map.entry('A', new Color(0x3CB44B)), // Green
            Map.entry('B', new Color(0x4363D8)), // Blue
            Map.entry('C', new Color(0xF58231)), // Orange
            Map.entry('D', new Color(0x911EB4)), // Purple
            Map.entry('E', new Color(0x46F0F0)), // Cyan
            Map.entry('F', new Color(0xF032E6)), // Magenta
            Map.entry('G', new Color(0xBCF60C)), // Lime
            Map.entry('H', new Color(0xFABEBE)), // Pink
            Map.entry('I', new Color(0x008080)), // Teal
            Map.entry('J', new Color(0xFFD8B1)), // Apricot
            Map.entry('K', new Color(0x000075)), // Very dark blue
            Map.entry('L', new Color(0x808000)), // Olive
            Map.entry('M', new Color(0xAA6E28)), // Brown
            Map.entry('N', new Color(0xA9A9A9)), // Dark gray
            Map.entry('O', new Color(0xDCBEFF)), // Lavender
            Map.entry('P', new Color(0xE6194B)), // Red
            Map.entry('Q', new Color(0x9A6324)), // Earth brown
            Map.entry('R', new Color(0xFFE119)), // Yellow
            Map.entry('S', new Color(0xBCEBCB)), // Mint
            Map.entry('T', new Color(0xFFB3BA)), // Soft pink
            Map.entry('U', new Color(0xC9C9FF)), // Light purple
            Map.entry('V', new Color(0xB9FBC0)), // Light green
            Map.entry('W', new Color(0x8B0000)), // Dark red
            Map.entry('X', new Color(0x40E0D0)), // Turquoise
            Map.entry('Y', new Color(0xFF69B4)), // Hot pink
            Map.entry('Z', new Color(0x5D3FD3)) // Deep purple
    );

    public static Color getColorForChar(char ch) {
        return fixedColorMap.getOrDefault(ch, Color.GRAY);
    }

    private BufferedImage renderBoard(char[][] board, int w, int h) {
        int cellSize = 40;
        BufferedImage image = new BufferedImage(w * cellSize, h * cellSize, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = image.createGraphics();

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                char c = board[i][j];
                Color color = c == '.' ? Color.WHITE : getColorForChar(c);

                g.setColor(color);
                g.fillRect(j * cellSize, i * cellSize, cellSize, cellSize);
                g.setColor(Color.BLACK);
                g.drawRect(j * cellSize, i * cellSize, cellSize, cellSize);

                if (c != '.' && c != ' ') {
                    g.setColor(Color.BLACK);
                    g.drawString(Character.toString(c), j * cellSize + 15, i * cellSize + 25);
                }
            }
        }

        g.dispose();
        return image;
    }

}
