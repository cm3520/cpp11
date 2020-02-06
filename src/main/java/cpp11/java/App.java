/*
 * This Java source file was generated by the Gradle 'init' task.
 */
package cpp11.java;

public class App extends SupperClass {
    static {
        System.loadLibrary("HelloWorld");
    }

    // for string
    private native void print();

    private native String getLine(String prompt);

    // for array primitive & object
    private native int sumArray(int[] arr);

    private native int[][] initInt2DArray(int size);

    // for Accessing Fields
    private String s;

    private native void accessField();

    // for Accessing Static Fields
    private static int si;

    // Calling Methods
    private native void nativeMethod();

    private void callback() {
        System.out.println("Calling In Java");
    }

    private static void scallback() {
        System.out.println("static Calling In Java");
    }

    // Calling Instance Methods of a Supperclass
    @Override
    public void show() {
        System.out.println("In App Class");
    }

    public static void main(String[] args) {
        App app = new App();

        // for string
        app.print();
        String input = app.getLine("Type a line: ");
        System.out.println(input);

        // for array
        int arr[] = new int[10];
        for (int i = 0; i < 10; ++i) {
            arr[i] = i;
        }

        int sum = app.sumArray(arr);
        System.out.println("sum = " + sum);

        int[][] i2arr = app.initInt2DArray(3);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++)
                System.out.print(" " + i2arr[i][j]);
            System.out.println();
        }

        // for Accessing Static Fields
        si = 100;

        // for Accessing Fields
        app.s = "Java layer";
        app.accessField();

        System.out.println("In Java:");
        System.out.println(" app.s = \"" + app.s + "\"" + "\n" + " App.si = " + si);

        // for Calling Methods
        app.nativeMethod();
    }
}
