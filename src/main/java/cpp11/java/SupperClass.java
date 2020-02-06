package cpp11.java;

public class SupperClass {

    // for Calling Instance Methods of a SupperClass
    public void show() {
        System.out.println("In SupperClass");
    }

    private void printString(String str) {
        System.out.println("From Native String: " + str);
    }
}