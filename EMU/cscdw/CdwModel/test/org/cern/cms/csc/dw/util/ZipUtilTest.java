package org.cern.cms.csc.dw.util;

import java.io.File;
import java.io.IOException;
import java.util.zip.ZipException;
import junit.framework.Assert;
import org.apache.log4j.Logger;
import org.junit.Test;

public class ZipUtilTest {

    private static final Logger logger = Logger.getLogger(ZipUtilTest.class);

    public ZipUtilTest() {
    }

    @Test
    public void unzipTest() {

        File zipFile = new File("src/graphdb/graphdb.zip");
        Assert.assertTrue("Zip file exists", zipFile.exists());

        File destFile = new File("build/zipTest");
        if (destFile.exists()) {
            ZipUtil.deleteRecursive(destFile);
        }
        destFile.mkdirs();

        Assert.assertTrue("Destination directory exists", destFile.exists());
        Assert.assertTrue("Destination directory is DIRECTORY", destFile.isDirectory());
        Assert.assertTrue("Destination directory is empty", destFile.listFiles().length == 0);
        try {
            
            ZipUtil.unzip(zipFile, destFile);

        } catch (ZipException ex) {
            logger.error("Test failed", ex);
            Assert.fail();
        } catch (IOException ex) {
            logger.error("Test failed", ex);
            Assert.fail();
        }
        
        Assert.assertTrue("Destination directory is not empty", destFile.listFiles().length > 0);

    }

}