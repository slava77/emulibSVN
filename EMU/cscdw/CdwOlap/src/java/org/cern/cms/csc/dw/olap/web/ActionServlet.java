package org.cern.cms.csc.dw.olap.web;

import com.tonbeller.jpivot.mondrian.MondrianMdxQuery;
import com.tonbeller.jpivot.mondrian.MondrianModel;
import com.tonbeller.jpivot.tags.OlapModelProxy;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import mondrian.olap.CacheControl;
import org.json.JSONArray;

public class ActionServlet extends HttpServlet {

    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {

        String id = request.getParameter("id");
        if (id != null) {

            if (id.equalsIgnoreCase("cubes")) {
                response.setContentType("text/plain");
                try {
                    JSONArray ja = new JSONArray(ConfigManager.getCubes());
                    response.getWriter().print(ja.toString());
                    return;
                } catch (Exception ex) {
                    throw new ServletException(ex);
                }
            }

            if (id.equalsIgnoreCase("reset")) {
                request.getSession().invalidate();
            }

            if (id.equalsIgnoreCase("refresh")) {
                refreshCache(request.getSession());
            }

            if (id.equalsIgnoreCase("select")) {
                String cube = request.getParameter("cube");
                if (cube != null) {
                    if (ConfigManager.getCubes().contains(cube)) {
                        setQuery(request.getSession(), ConfigManager.getDefaultQuery(cube));
                    }
                }
            }
            
        }

        response.sendRedirect("index.jsp");
    } 

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    } 

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        processRequest(request, response);
    }

    private String getQuery(HttpSession session) {
        OlapModelProxy query01 = (OlapModelProxy) session.getAttribute("query01");
        MondrianMdxQuery mdxQuery = (MondrianMdxQuery) query01.getExtension("mdxQuery");
        return mdxQuery.getMdxQuery();
    }

    private void setQuery(HttpSession session, String query) {
        OlapModelProxy query01 = (OlapModelProxy) session.getAttribute("query01");
        MondrianMdxQuery mdxQuery = (MondrianMdxQuery) query01.getExtension("mdxQuery");
        mdxQuery.setMdxQuery(query);
    }

    private void refreshCache(HttpSession session) {
        OlapModelProxy query01 = (OlapModelProxy) session.getAttribute("query01");
        MondrianMdxQuery mdxQuery = (MondrianMdxQuery) query01.getExtension("mdxQuery");
        MondrianModel mm = (MondrianModel) mdxQuery.getModel();
        CacheControl cc = mm.getConnection().getCacheControl(null);
        cc.flushSchema(mm.getConnection().getSchema());
    }

}
