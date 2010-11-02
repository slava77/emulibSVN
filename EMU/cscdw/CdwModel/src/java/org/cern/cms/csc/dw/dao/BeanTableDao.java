package org.cern.cms.csc.dw.dao;

import java.util.ArrayList;
import org.cern.cms.csc.dw.dao.table.BeanTableIf;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;
import org.cern.cms.csc.dw.dao.table.BeanTableProjectionFilterItemIf;
import org.cern.cms.csc.dw.dao.table.BeanTableSortColumnIf;
import org.cern.cms.csc.dw.util.SQLParamRestriction;
import org.hibernate.Criteria;
import org.hibernate.QueryException;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.criterion.DetachedCriteria;
import org.hibernate.criterion.Junction;
import org.hibernate.criterion.Order;
import org.hibernate.criterion.Projections;
import org.hibernate.criterion.Property;
import org.hibernate.criterion.Restrictions;
import org.hibernate.criterion.Subqueries;
import org.hibernate.ejb.EntityManagerImpl;

@Stateless
public class BeanTableDao implements BeanTableDaoLocal {

    private static final Pattern filterKeyPattern;
    private static Matcher filterKeyMatcher;

    static {
        filterKeyPattern = Pattern.compile("^.*:");
        filterKeyMatcher = filterKeyPattern.matcher("");
    }

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public List getData(BeanTableIf table, boolean pageOnly) {

        Session session = (Session)((EntityManagerImpl)em.getDelegate()).getDelegate();
        Transaction transaction = session.beginTransaction();

        Criteria c = getCriteria(session, table);

        for (BeanTableSortColumnIf sc: table.getSortByColumns()) {
            if (sc.isAscending()) {
                c.addOrder(Order.asc(sc.getPropertyName()));
            } else {
                c.addOrder(Order.desc(sc.getPropertyName()));
            }
        }

        if (pageOnly) {
            c.setFirstResult((table.getPageIndex() - 1) * table.getPageSize());
            c.setMaxResults(table.getPageSize());
        }

        c.setCacheable(true);
        c.setCacheRegion(table.getRowClass().getCanonicalName());

        List list = new ArrayList();
        try {
            list = c.list();

        } catch (QueryException ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
            if (table.isInteractiveMode()) {
                //BeanBase.addErrorMessage("cms.dqm.workflow.getData.ERROR", false);
            }
        }

        transaction.rollback();

        return list;
    }

    public Long getDataCount(BeanTableIf table) {

        Session session = (Session)((EntityManagerImpl)em.getDelegate()).getDelegate();
        Transaction transaction = session.beginTransaction();

        Long count = ((Integer) getCriteria(session, table)
                .setProjection(Projections.rowCount())
                .setCacheable(true)
                .setCacheRegion(table.getRowClass()
                .getCanonicalName())
                .uniqueResult()).longValue();

        transaction.rollback();

        return count;
    }

    private Criteria getCriteria(Session session, BeanTableIf table) {
        return getDetachedCriteria(table).getExecutableCriteria(session);
    }

    private Criteria getCriteria(Session session, Class rowClass, Map<String, BeanTableFilterIf> filter) {
        return getDetachedCriteria(rowClass, filter, null).getExecutableCriteria(session);
    }

    private DetachedCriteria getDetachedCriteria(BeanTableIf table) {
        return getDetachedCriteria(table.getRowClass(), table.getFilter(), table.getAdvancedQuery());
    }

    private DetachedCriteria getDetachedCriteria(Class rowClass, Map<String, BeanTableFilterIf> filter, String advancedQuery) {

        DetachedCriteria c = DetachedCriteria.forClass(rowClass);

        if (advancedQuery != null) {
            c.add(SQLParamRestriction.restriction(advancedQuery));
        }

        for (Iterator<String> filterItr = filter.keySet().iterator(); filterItr.hasNext();) {

            String k = filterItr.next();
            BeanTableFilterIf f = filter.get(k);

            if (f.getItems().size() > 0) {

                filterKeyMatcher.reset(k);
                k = filterKeyMatcher.replaceAll("");

                Junction disJun = Restrictions.disjunction();
                Junction conJun = Restrictions.conjunction();
                Junction curJun = disJun;

                for (Iterator<BeanTableFilterItemIf> filterItemItr = f.getItems().iterator(); filterItemItr.hasNext();) {

                    BeanTableFilterItemIf item = filterItemItr.next();

                    switch (item.getOperator()) {
                        case AND:
                            curJun = conJun;
                            break;
                        case OR:
                            curJun = disJun;
                            break;
                    }

                    switch (item.getOperation()) {
                        case EQUAL:
                            curJun.add(Restrictions.eq(k, item.getValue()));
                            break;
                        case MORE:
                            curJun.add(Restrictions.gt(k, item.getValue()));
                            break;
                        case LESS:
                            curJun.add(Restrictions.lt(k, item.getValue()));
                            break;
                        case MORE_EQUAL:
                            curJun.add(Restrictions.ge(k, item.getValue()));
                            break;
                        case LESS_EQUAL:
                            curJun.add(Restrictions.le(k, item.getValue()));
                            break;
                        case NOT_EQUAL:
                            curJun.add(Restrictions.ne(k, item.getValue()));
                            break;
                        case LIKE:
                            curJun.add(Restrictions.like(k, item.getValue()));
                            break;
                        case NOTLIKE:
                            curJun.add(Restrictions.not(Restrictions.like(k, item.getValue())));
                            break;
                        case ISNULL:
                            curJun.add(Restrictions.isNull(k));
                            break;
                        case ISNOTNULL:
                            curJun.add(Restrictions.isNotNull(k));
                            break;
                        case IN:
                            if (item instanceof BeanTableProjectionFilterItemIf) {
                                BeanTableProjectionFilterItemIf pitem = (BeanTableProjectionFilterItemIf) item;
                                BeanTableIf subQueryTable = (BeanTableIf) item.getValue();
                                if (subQueryTable.isFilterOn() || subQueryTable.isAdvancedQuerySet()) {
                                    DetachedCriteria subCriteria = getDetachedCriteria(subQueryTable);
                                    subCriteria.setProjection(Property.forName(pitem.getProperty()));
                                    curJun.add(Subqueries.propertyIn(k, subCriteria));
                                }
                            }
                            break;
                    }

                }

                c.add(disJun).add(conJun);

            }

        }

        return c;

    }

    /*
    public String getCriteriaFilterWhereClause(BeanTableIf table) throws NoSuchFieldException {

        StringBuilder sql = new StringBuilder();
        Class rowClass = table.getRowClass();

        for (Iterator<String> filterItr = table.getFilter().keySet().iterator(); filterItr.hasNext();) {

            String k = filterItr.next();
            BeanTableFilterIf f = table.getFilter().get(k);

            if (f.getItems().size() > 0) {

                filterKeyMatcher.reset(k);
                k = filterKeyMatcher.replaceAll("");

                StringBuffer subsql = new StringBuffer();

                for (Iterator<BeanTableFilterItemIf> filterItemItr = f.getItems().iterator(); filterItemItr.hasNext();) {
                    BeanTableFilterItemIf item = filterItemItr.next();
                    switch (item.getOperator()) {
                        case AND:
                            if (subsql.length() > 0) {
                                subsql.append(" AND ");
                            }
                            break;
                        case OR:
                            if (subsql.length() > 0) {
                                subsql.append(" OR ");
                            }
                            break;
                    }

                    Field field = rowClass.getDeclaredField(k);
                    Column fieldColumn = field.getAnnotation(Column.class);
                    subsql.append(fieldColumn.name());

                    boolean skipValue = false;

                    switch (item.getOperation()) {
                        case NOTLIKE:
                            subsql.append(" NOT LIKE ");
                            break;
                        case ISNULL:
                            subsql.append(" IS NULL ");
                            skipValue = true;
                            break;
                        case ISNOTNULL:
                            subsql.append(" IS NOT NULL ");
                            skipValue = true;
                            break;
                        default:
                            subsql.append(" ")
                                  .append(item.getOperation().getValue())
                                  .append(" ");
                    }

                    if (!skipValue) {

                        Class fieldType = field.getType();

                        if (fieldType.equals(String.class)) {
                            String v = (String) item.getValue();
                            subsql.append("'");
                            subsql.append(v.replaceAll("'", "''"));
                            subsql.append("'");
                        } else {
                            if (fieldType.equals(Boolean.class)) {
                                Boolean v = (Boolean) item.getValue();
                                subsql.append(v ? 1 : 0);
                            } else {
                                subsql.append(item.getValue());
                            }
                        }
                    }

                }

                if (sql.length() > 0) {
                    sql.append(" AND ");
                }
                sql.append("(");
                sql.append(subsql);
                sql.append(")");

            }

        }

        return sql.toString();
    }
    */

}
