package org.cern.cms.csc.dw.util;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;

public class ZipUtil {

    private static final void copyInputStream(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        int len;
        while ((len = in.read(buffer)) >= 0) {
            out.write(buffer, 0, len);
        }
        in.close();
        out.close();
    }

    public static void unzip(File fileToUnzip, File destDir) throws ZipException, IOException {
        ZipFile zipFile = new ZipFile(fileToUnzip);

        if (destDir.exists()) {
            if (!destDir.isDirectory()) {
                throw new IOException("Destination must be a directory!");
            }
        } else {
            destDir.mkdirs();
        }

        Enumeration<? extends ZipEntry> entries = zipFile.entries();
        List<ZipEntry> files = new LinkedList<ZipEntry>();

        // Extracting entries and creating directories
        while (entries.hasMoreElements()) {
            ZipEntry entry = entries.nextElement();
            
            if (entry.isDirectory()) {
                (new File(destDir.getAbsolutePath() + File.separator + entry.getName())).mkdirs();
                continue;
            }

            files.add(entry);
        }

        // Creating files
        for (ZipEntry entry: files) {
            copyInputStream(zipFile.getInputStream(entry),
                    new BufferedOutputStream(new FileOutputStream(destDir.getAbsolutePath() + File.separator + entry.getName())));
        }

        zipFile.close();
    }

    public static void deleteRecursive(File file) {
        if (file.isFile()) {
            file.delete();
        } else
        if (file.isDirectory()) {
            for (File f: file.listFiles()) {
                deleteRecursive(f);
            }
        }
    }

}
