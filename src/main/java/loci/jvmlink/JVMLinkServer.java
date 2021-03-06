/*
 * #%L
 * JVMLink client/server architecture for communicating between Java and
 * non-Java programs using sockets.
 * %%
 * Copyright (C) 2008 - 2014 Board of Regents of the University of
 * Wisconsin-Madison.
 * %%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * #L%
 */

package loci.jvmlink;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

/**
 * Java server for JVMLink. Clients can connect to the server and issue
 * arbitrary Java commands according to the legal syntax of the
 * {@link org.scijava.util.ReflectedUniverse} class.
 *
 * @author Hidayath Ansari
 */
public class JVMLinkServer implements Runnable {

  // -- Constants --

  private static final int DEFAULT_PORT = 20345;
  private static final int MIN_PORT = 1024;
  private static final int MAX_PORT = 65535;
  private static final int TIMEOUT = 500;

  // -- Static fields --

  /** Debugging flag. */
  private boolean debug;

  // -- Fields --

  private int port;
  private boolean alive = true;
  private ServerSocket listener;

  // -- Constructor --

  public JVMLinkServer(int port) {
    this.port = port;
  }

  // -- JVMLinkServer API methods --

  /** Orders the server to begin listening for incoming connections. */
  public void listen() throws IOException {
    if (listener == null) {
      listener = new ServerSocket(port);
      listener.setSoTimeout(TIMEOUT);
      new Thread(this, "JVMLink-Listener").start();
    }
  }

  /** Orders the server to terminate. */
  public void shutServer() throws IOException {
    debug("Shutting down");
    alive = false;
    listener.close();
  }

  public void setDebug(boolean debug) {
    this.debug = debug;
  }

  public boolean getDebug() {
    return this.debug;
  }

  // -- Runnable API methods --

  /** Continually listens for incoming client connections. */
  public void run() {
    System.out.println("JVMLink server started on port " + port);
    while (alive) {
      try {
        Socket socket = listener.accept();
        debug("Got a connection from " +
          socket.getInetAddress().getHostName());
        new ConnThread(socket, this);
      }
      catch (SocketException exc) {
        // socket was probably closed; terminate server
        debug("Server terminated");
        break;
      }
      catch (SocketTimeoutException exc) {
        //if (debug) System.out.print(".");
      }
      catch (IOException exc) {
        if (debug) exc.printStackTrace();
        try {
          Thread.sleep(100);
        }
        catch (InterruptedException exc2) {
          if (debug) exc2.printStackTrace();
        }
      }
    }
  }

  // -- Helper methods --

  public void debug(String msg) {
    if (debug) System.out.println("JVMLinkServer: " + msg);
  }

  // -- Main method --

  public static void main(String[] args) throws IOException {
    boolean debug = false;
    int port = DEFAULT_PORT;
    for (int i=0; i<args.length; i++) {
      if ("-debug".equals(args[i])) {
        debug = true;
      } else {
        try {
          int num = Integer.parseInt(args[i]);
          if (num < MIN_PORT || num > MAX_PORT) {
            System.err.println("Invalid port: " + num);
            return;
          }
          else port = num;
        }
        catch (NumberFormatException exc) {
          System.err.println("Unknown parameter: " + args[i]);
          return;
        }
      }
    }
    JVMLinkServer ns = new JVMLinkServer(port);
    ns.setDebug(debug);
    ns.listen();
  }

}
