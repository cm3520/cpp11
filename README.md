# cpp11
Test Code For C++11 Features & JNI

[TOC]

## JNI
### 类型转换
#### 字符串转换
	Your native method code must use the appropriate JNI functions to convert jstring objects to C/C++ strings. The JNI supports conversion both to and from Unicode and UTF-8 strings.
1. GetStringUTFChars:虚拟机会分配内存去存储UTF-8字符串，所以需要check该函数的返回值。
	The GetStringUTFChars function is available through the JNIEnv interface pointer. It converts the jstring reference, typically represented by the Java virtual machine implementation as a Unicode sequence, into a C string represented in the UTF-8 format.
2. ReleaseStringUTFChars：释放内存
	 Calling ReleaseStringUTFChars indicates that the native method no longer needs the UTF-8 string returned by GetStringUTFChars; thus the memory taken by the UTF-8 string can be freed.
3. NewStringUTF：构建一个新的字符串
	You can construct a new java.lang.String instance in the native method by calling the JNI function NewStringUTF. The NewStringUTF function takes a C string with the UTF-8 format and constructs a java.lang.String instance. The newly constructed java.lang.String instance represents the same sequence of Unicode characters as the given UTF-8 C string.

**UTF-8 strings are always terminated with the ‘\0’ character, whereas Unicode strings are not.**
1. GetStringChars & ReleaseStringChars & GetStringLength & GetStringUTFLength
	GetStringChars and ReleaseStringChars obtain string characters represented in the Unicode format.
	When the location pointed to by isCopy is set to JNI_FALSE, native code must not modify the contents of the returned string.
	Do not forget to call ReleaseStringChars when you no longer need access to the string elements returned from GetStringChars. The ReleaseStringChars call is necessary whether GetStringChars has set *isCopy to JNI_TRUE or JNI_FALSE. ReleaseStringChars either frees the copy or unpins the instance, depending upon whether GetStringChars has returned a copy or not.

**critical region**
1. Get/ReleaseStringCritical
	Native code between a Get/ReleaseStringCritical pair must not issue blocking calls or allocate new objects in the Java virtual machine.Otherwise, the virtual machine may deadlock.
2. GetStringUTFRegion:不分配内存
	The code is somewhat simpler than using GetStringUTFChars. Because GetStringUTFRegion performs no memory allocation, we need not check for possible out-of-memory conditions.

![Table](Summary_of_JNI_String_Functions.png)

#### 数组转换
##### primitive
	The JNI treats primitive arrays and object arrays differently. Primitive arrays contain elements that are of primitive types such as int and boolean. Object arrays contain elements that are of reference types such as class instances and other arrays.
	Accessing primitive arrays in a native method requires the use of JNI functions similar to those used for accessing strings.
1. GetIntArrayRegion & SetIntArrayRegion
	Using the GetIntArrayRegion function to copy all the elements in the integer array into a C buffer (buf).
	The JNI supports a corresponding SetIntArrayRegion function that allows native code to modify the array elements of type int.
2. Get/Release<Type>ArrayElements
	The JNI supports a family of Get/Release<Type>ArrayElements functions (including, for example, Get/ReleaseIntArrayElements) that allow the native code to obtain a direct pointer to the elements of primitive arrays.

![Table](Summary_of_JNI_Primitive_Array_Functions.png)

##### objects
1. Get/SetObjectArrayElement
	The JNI provides a separate pair of functions to access objects arrays. GetObjectArrayElement returns the element at a given index, whereas SetObjectArrayElement updates the element at a given index.

### Fields and Methods
#### Accessing Fields
	The Java programming language supports two kinds of fields. Each instance of a class has its own copy of the instance fields of the class, whereas all instances of a class share the static fields of the class.

1. Procedure for Accessing an Instance Field
	To access an instance field, the native method follows a two-step process. First, it calls **GetFieldID** to obtain the field ID from the class reference, field name, and field descriptor。Once you have obtained the field ID, you can pass the object reference and the field ID to the appropriate instance field access function。
    GetObjectClass（通过jobject获取实例的类）-> GetFieldID(获取类的FieldID) -> GetObjectField（获取当前实例jobject的FieldID中存储的值）-> 如果获取的值不是基本类型（像String或array），则还需进一步进行转换。

2. Field Descriptors
	we used a specially encoded C string "Ljava/lang/String;" to represent a field type in the Java programming language. These C strings are called JNI field descriptors. you represent an int field with "I", a float field with "F", a double
field with "D", a boolean field with "Z", and so on.
	The descriptor for a reference type, such as java.lang.String, begins with the letter L, is followed by the JNI class descriptor, and is terminated by a semicolon。The “.” separators in fully qualified class names are changed to “/” in
JNI class descriptors.
	Descriptors for array types consist of the “[” character, followed by the descriptor of the component type of the array.
	You can use the javap tool to generate the field descriptors from class files. Normally javap prints out the method and field types in a given class. If you specify the -s option (and the -p option for exposing private members), javap prints JNI descriptors instead:
    ~~~cpp
    javap -s -p InstanceFieldAccess
    ~~~

3. Accessing Static Fields
	There are two differences between how you access a static field and how you access an instance field:
	1. You call GetStaticFieldID for static fields, as opposed to GetFieldID for instance fields. GetStaticFieldID and GetFieldID have the same return type jfieldID.
	2. Once you have obtained the static field ID, you pass the class reference, as opposed to an object reference, to the appropriate static field access function.

#### Calling Methods
	There are several kinds of methods in the Java programming language. Instance methods must be invoked on a specific instance of a class, whereas static methods may be invoked independent of any instance.
1. You can use Call<Type>Method family of functions to invoke interface methods as well.
	the two steps required to call an instance method:
	1. The native method first calls the JNI function GetMethodID.
	2. The native method then calls CallVoidMethod.
	You can use Call<Type>Method family of functions to invoke interface methods as well.
2. Calling Static Methods
	you can perform callbacks to static methods from native code by following these steps:
    1. Obtain the method ID using GetStaticMethodID, as opposed to GetMethodID.
    2. Pass the class, method ID, and arguments to one of the family of static method invocation functions: 	CallStaticVoidMethod, CallStaticBooleanMethod, and so on.
3. Calling Instance Methods of a Superclass
	You can call instance methods which were defined in a superclass but that have been overridden in the class to which the object belongs. The JNI provides a set of CallNonvirtual<Type>Method functions for this purpose. To call a instance method defined in a superclass, you do the following:
    1. Obtain the method ID from a reference to the superclass using GetMethodID, as opposed to GetStaticMethodID.
	2. Pass the object, superclass, method ID, and arguments to one of the family of nonvirtual invocation functions, such as CallNonvirtualVoidMethod, CallNonvirtualBooleanMethod, and so on.
4. Invoking Constructors
	In the JNI, constructors may be invoked following steps similar to those used for calling instance methods. To obtain the method ID of a constructor, pass "<init>" as the method name and "V" as the return type in the method descriptor.You can then invoke the constructor by passing the method ID to JNI functions such as NewObject.
    FindClass -> GetMethodID -> NewObject
5. Caching Field and Method IDs
	Obtaining field and method IDs requires symbolic lookups based on the name and descriptor of the field or method. Symbolic lookups are relatively expensive. In this section, we introduce a technique that can be used to reduce this overhead: The idea is to compute field and method IDs and cache them for repeated uses later.
    There are two ways to cache field and method IDs, depending upon whether caching is performed at the point of use of the field or method ID, or in the static initializer of the class that defines the field or method.
	1. Caching at the Point of Use
	Field and method IDs may be cached at the point where native code accesses the field values or performs method callbacks.
	2. Caching in the Defining Class’s Initializer
	In many situations it is more convenient to initialize the field and method IDs required by a native method before the application can have a chance to invoke the native method.
	Comparison between the Two Approaches to Caching IDs:
    1. caching at the point of use requires a check in the execution fast path and may also require duplicated checks and initialization of the same field or method ID.
    2. Method and field IDs are only valid until the class is unloaded.









## CPP11
