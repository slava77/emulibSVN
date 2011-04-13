package jsf.bean.gui;

import com.icesoft.faces.context.effects.JavascriptContext;
import java.io.Serializable;
import javax.faces.FactoryFinder;
import javax.faces.application.Application;
import javax.faces.application.FacesMessage;
import javax.faces.application.ViewHandler;
import javax.faces.component.UIComponent;
import javax.faces.component.UIViewRoot;
import javax.faces.context.ExternalContext;
import javax.faces.context.FacesContext;
import javax.faces.context.FacesContextFactory;
import javax.faces.event.ActionEvent;
import javax.faces.lifecycle.Lifecycle;
import javax.faces.lifecycle.LifecycleFactory;
import javax.faces.validator.ValidatorException;
import javax.persistence.Transient;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public abstract class JsfBeanBase implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(JsfBeanBase.class);

    public JsfBeanBase() {
    }

    @Transient
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

    @Transient
    public static Object getBean(String expr) {
        return getEvalValue(expr);
    }

    @Transient
    public static Object getEvalValue(String expr) {
        return getEvalValue(FacesContext.getCurrentInstance(), expr);
    }

    @Transient
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

    @Transient
    public static Object getParameter(String name) {
        return getExternalContext().getRequestParameterMap().get(name);
    }

    @Transient
    public static HttpServletRequest getRequest() {
        return (HttpServletRequest) getExternalContext().getRequest();
    }

    @Transient
    public static HttpServletResponse getResponse() {
        return (HttpServletResponse) getExternalContext().getResponse();
    }

    @Transient
    public static String getRequestParameter(String key) {
        return getExternalContext().getRequestParameterMap().get(key);
    }

    @Transient
    public static boolean isRequestParameterExists(String key) {
        return getExternalContext().getRequestParameterMap().containsKey(key);
    }

    @Transient
    public static ExternalContext getExternalContext() {
        FacesContext facesContext = FacesContext.getCurrentInstance();
        return facesContext.getExternalContext();
    }

    @Transient
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

    @Transient
    public static void setCookie(String name, String value, int age) {
        Cookie cookie = new Cookie(name, value);
        cookie.setMaxAge(age);
        getResponse().addCookie(cookie);
    }

    @Transient
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

    public static void addInfoMessage(String msg, Object... args) {
        addMessage(FacesMessage.SEVERITY_INFO, null, msg, args);
    }

    public static void addErrorMessage(Throwable th) {
        StringBuilder sb = new StringBuilder();
        while (th != null) {
            if (sb.length() > 0) {
                sb.append("\n");
            }
            sb.append(th.getMessage());
            th = th.getCause();
        }
        addMessage(FacesMessage.SEVERITY_ERROR, null, sb.toString());
    }

    public static void addErrorMessage(String msg, Object... args) {
        addMessage(FacesMessage.SEVERITY_ERROR, null, msg, args);
    }

    public static void addErrorMessage(UIComponent component, String msg, Object... args) {
        addMessage(FacesMessage.SEVERITY_ERROR, component, msg, args);
    }

    public static void addMessageEx(FacesMessage.Severity severity, UIComponent component, String msg, Object... args) throws ValidatorException {
        FacesMessage message = addMessage(severity, component, msg, args);
        throw new ValidatorException(message);
    }

    public static FacesMessage addMessage(FacesMessage.Severity severity, UIComponent component, String msg, Object... args) {
        String componentId = null;
        if (component != null) {
            componentId = component.getClientId(FacesContext.getCurrentInstance());
        }
        if (args.length > 0) {
            msg = String.format(msg, args);
        }
        FacesMessage message = new FacesMessage(severity, msg, msg);
        FacesContext.getCurrentInstance().addMessage(componentId, message);
        return message;
    }

}
