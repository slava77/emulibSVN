package org.cern.cms.csc.dw.model.ontology.graph;

import org.neo4j.graphdb.GraphDatabaseService;
import org.neo4j.graphdb.Node;
import org.neo4j.graphdb.Transaction;
import org.neo4j.index.lucene.LuceneFulltextIndexService;
import org.neo4j.index.lucene.LuceneFulltextQueryIndexService;
import org.neo4j.index.lucene.LuceneIndexService;
import org.neo4j.kernel.EmbeddedGraphDatabase;

public abstract class GServicesBase {

    private GraphDatabaseService db;
    private LuceneIndexService idx;
    private LuceneFulltextIndexService ftidx;
    private LuceneFulltextQueryIndexService ftqidx;

    protected GServicesBase(String path) {
        db = new EmbeddedGraphDatabase(path);
        idx = new LuceneIndexService(db);
        ftidx = new LuceneFulltextIndexService(db);
        ftqidx = new LuceneFulltextQueryIndexService(db);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    public void close() {
        ftqidx.shutdown();
        ftidx.shutdown();
        idx.shutdown();
        db.shutdown();
    }

    public GraphDatabaseService getDBSrv() {
        return db;
    }

    public LuceneIndexService getIdxSrv() {
        return idx;
    }

    public LuceneFulltextIndexService getFTIdxSrv() {
        return ftidx;
    }

    public LuceneFulltextQueryIndexService getFTQueryIdxSrv() {
        return ftqidx;
    }

    public Node getReferenceNode() {
        Transaction tx = beginTx();
        try {
            return db.getReferenceNode();
        }
        finally {
            tx.finish();
        }
    }

    public Transaction beginTx() {
        return db.beginTx();
    }

}
