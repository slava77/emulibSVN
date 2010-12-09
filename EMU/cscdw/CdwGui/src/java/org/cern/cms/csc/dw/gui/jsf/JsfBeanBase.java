package org.cern.cms.csc.dw.gui.jsf;

import com.icesoft.faces.context.effects.JavascriptContext;
import javax.faces.FactoryFinder;
import javax.faces.application.Application;
import javax.faces.application.ViewHandler;
import javax.faces.component.UIViewRoot;
import javax.faces.context.ExternalContext;
import javax.faces.context.FacesContext;
import javax.faces.context.FacesContextFactory;
import javax.faces.event.ActionEvent;
import javax.faces.lifecycle.Lifecycle;
import javax.faces.lifecycle.LifecycleFactory;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import org.apache.log4j.Logger;

public abstract class JsfBeanBase {

    private static final Logger logger = Logger.getLogger(JsfBeanBase.class);

    public static FacesContext getFacesContext(HttpServletRequest request, HttpServletResponse response, String lifecycleId) {
        FacesContext facesContext = FacesContext.getCurrentInstance();
        if (facesContext == null) {

            LifecycleFactory lifecycleFactory = (LifecycleFactory) FactoryFinder.getFactory(FactoryFinder.LIFECYCLE_FACTORY);
            if (lifecycleId == null) {
                lifecycleId = LifecycleFactory.DEFAULT_LIFECYCLE;
            }
            Lifecycle lifecycle = lifecycleFactory.getLifecycle(lifecycleId);

            FacesContextFactory contextFactory = (FacesContextFactory) FactoryFinder.getFactory(FactoryFinder.FACES_CONTEXT_FACTORY);
            facesContext = contextFactory.getFacesContext(request.getSession().getServletContext(), request, response, lifecycle);

            // Create new View.
            UIViewRoot view = facesContext.getApplication().getViewHandler().createView(facesContext, "");
            facesContext.setViewRoot(view);

        }
        return facesContext;
    }

    public static Object getBean(String expr) {
        return getEvalValue(expr);
    }

    public static Object getEvalValue(String expr) {
        return getEvalValue(FacesContext.getCurrentInstance(), expr);
    }

    public static Object getEvalValue(FacesContext context, String expr) {
        Object value = null;
        try {
            Application app = context.getApplication();
            value = app.evaluateExpressionGet(context, "#{" + expr + "}", Object.class);
        } catch (NullPointerException e) {
            logger.error("Error while evaluating expression", e);
        }
        return value;
    }

    public static Object getParameter(String name) {
        return getExternalContext().getRequestParameterMap().get(name);
    }

    public static HttpServletRequest getRequest() {
        return (HttpServletRequest) getExternalContext().getRequest();
    }

    public static HttpServletResponse getResponse() {
        return (HttpServletResponse) getExternalContext().getResponse();
    }

    public static String getRequestParameter(String key) {
        return getExternalContext().getRequestParameterMap().get(key);
    }

    public static ExternalContext getExternalContext() {
        FacesContext facesContext = FacesContext.getCurrentInstance();
        return facesContext.getExternalContext();
    }

    public static HttpSession getSession(boolean create) {
        return (HttpSession) getExternalContext().getSession(create);
    }

    public static void refreshView() {
        FacesContext context = FacesContext.getCurrentInstance();
        Application application = context.getApplication();
        ViewHandler viewHandler = application.getViewHandler();
        UIViewRoot viewRoot = viewHandler.createView(context, context.getViewRoot().getViewId());
        context.setViewRoot(viewRoot);
        context.renderResponse();
    }

    public static void refreshViewListener(ActionEvent e) {
        refreshView();
    }

    public static void setCookie(String name, String value, int age) {
        Cookie cookie = new Cookie(name, value);
        cookie.setMaxAge(age);
        getResponse().addCookie(cookie);
    }

    public static String getCookie(String name) {
        Cookie cookie[] = getRequest().getCookies();
        if (cookie != null && cookie.length > 0) {
            for (int i = 0; i < cookie.length; i++) {
                if (cookie[i].getName().equals(name)) {
                    return cookie[i].getValue();
                }
            }
        }
        return null;
    }

   public static void addJavascript(String script) {
        JavascriptContext.addJavascriptCall(FacesContext.getCurrentInstance(), script);
   }

}
