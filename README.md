# JVMLink

> Work on JVMLink has ceased in favor of other solutions.  
> See [Interfacing from non-Java code](https://uw-loci.github.io/interfacing-non-java-code)
> page for more details.

JVMLink is a micro client/server architecture for communicating between
a Java Virtual Machine and other programs (e.g., C++ applications) via
TCP/IP sockets. JVMLink allows software developers to invoke Java code
from native code or other non-Java program. (It could also be used to
communicate between JVMs, though Java has other tools more tailored to
that, such as Java RMI.) JVMLink works either locally (via localhost) or
remotely.

## Purpose

JVMLink was written to quickly harness large Java efforts from within
native code. In particular, JVMLink allows native applications to call
[Bio-Formats](https://eliceirilab.org/software/bio-formats) to read
microscopy image data in over 130 formats.

## License

JVMLink is licensed under a BSD-style license, meaning it may be freely
used within commercial applications or modified as long as our copyright
notice is maintained.

## Rationale

There are a plethora of programs for connecting Java to other
infrastructures (do a Google search for [java
bridge](https://www.google.com/search?q=java+bridge) and see for
yourself). However, these projects typically concentrate on linking Java
to one specific platform, be it a language such as
[PHP](http://php-java-bridge.sourceforge.net/pjb/) or
[Python](http://jpype.sourceforge.net/), a platform such as Windows, or a
specific technology such as [COM](http://sourceforge.net/projects/jcom/).

Other projects such as [IKVM.NET](http://www.ikvm.net/) seek
interoperability through more drastic measures like reimplementing the
entire Java specification. While such an approach has the advantage of
providing total integration, it also comes at a cost: Java is a massive
platform and implementing it in its entirety is an enormous project.
IKVM.NET does not claim to support the entire specification—in
particular, according to the IKVM.NET website, the Java windowing
toolkits are not supported: "AWT and Swing are not presently functional.
This is a low priority item for the project developers."

More importantly, Sun has worked tirelessly for years to make the
official Java implementation performant and robust, and it is constantly
evolving and improving. Getting any large Java project to compile and
run under a different Java implementation is difficult. The alternative
implementation will almost certainly have its own bugs and shortcomings
that prevent the Java project from working out of the box. At least that
has been our experience trying to get
[Bio-Formats](https://eliceirilab.org/software/bio-formats) to work
with unofficial implementations of the JVM.

For these reasons, we abandoned the above approaches in favor of a much
simpler, but still very powerful, solution. By using a Java server that
accepts commands and data over a network socket, it becomes possible for
client applications on any platform to invoke Java code and receive
results over the wire. The result is JVMLink.

## Features

The server portion of JVMLink is a tiny Java application that opens a
single port (your choice, default is 20345) and listens for connections
from interested clients. Once a connection is established, the client
can send Java commands to execute on the server side via reflection, as
well as pass data back and forth between client and server. The current
version supports the eight primitive Java types plus Strings, as well as
arrays thereof.

The client portion should be implemented in whichever language you wish
to support. We provide a client implemention written in Microsoft Visual
C++ for Windows, but it should be straightforward to implement a JVMLink
client for cross-platform C, Python or other language. If you do
implement your own client and are interested in sharing, we would be
glad to include it as part of the official JVMLink package.

## Applications

We have deployed JVMLink within our native
[WiscScan](https://eliceirilab.org/software/wiscscan) acquisition
program to interface with the [ImageJ](https://imagej.net/) Java image
analysis application, feeding flow cytometry images in real time from
the native code to the Java side so that we can harness ImageJ's
particle counting features directly.

## Caveats

Please note that JVMLink currently contains **NO SECURITY FEATURES OF
ANY KIND**. This means that while the JVMLink server is running, **ANY
APPLICATION COULD CONNECT TO THE OPEN PORT AND ISSUE ARBITRARY
COMMANDS** to your server. Malicious software could do anything
permitted by an instance of Java running as whichever user started the
server, including deleting or changing files.

## Download

- [jvmlink.jar](https://maven.scijava.org/service/local/artifact/maven/redirect?r=snapshots&g=loci&a=jvmlink&v=LATEST&e=jar)

## Installation and Usage

To try out the client/server connection in Visual Studio, perform the
following steps:

1.  You will need the source code for the Visual C++ client. You can
    access it from our [Git
    repository](https://github.com/uw-loci/jvmlink).

2.  From within Visual Studio, open up either **cpp\\JVMLink.sln** or
    **cpp\\JVMLink.vcproj**.

3.  Compile the JVMLink server JAR using Maven.

4.  Make sure you have the [Java Runtime](https://java.com/) or [Java
    SDK](https://adoptopenjdk.net/) installed on your system. You can
    test whether you have Java available by opening a Command Prompt
    and typing the following command:
    ```
    java -jar C:\\jvmlink\\target\\jvmlink-1.0.0-SNAPSHOT.jar
    ```
    where "C:\\jvmlink" is the folder containing the JVMLink code. If
    all goes well you should see the message "JVMLink server started on
    port 20345"—you can then kill the server by pressing Ctrl+C, as the
    C++ client can launch its own copy of the server.

5.  To change the commands sent to Java, edit the section of TestC2.cpp
    that repeatedly calls JVMLinkClient's exec method. Note that the
    syntax, while as similar as possible to full-blown Java, is actually
    a simple scripting language that the server executes using Java's
    reflection capabilities.

6.  Back in Visual Studio, build and run the project. It should produce
    output similar to the screenshot above.

7.  You can toggle debugging output on the server side by twiddling the
    comments in the `startJava` method of **JVMLinkClient.cpp**. To
    toggle verbose debugging output on the client side, twiddle the
    `#define debug` lines near the top of **JVMLinkClient.h**.
