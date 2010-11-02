package org.cern.cms.csc.dw.util;

import org.hibernate.Criteria;
import org.hibernate.HibernateException;
import org.hibernate.criterion.Criterion;
import org.hibernate.criterion.CriteriaQuery;
import org.hibernate.engine.TypedValue;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class SQLParamRestriction implements Criterion {

    private final String sql;
    private static Pattern paramPattern = Pattern.compile("\\{([a-zA-Z0-9_]+)\\}");

    @Override
    public String toSqlString(Criteria criteria, CriteriaQuery criteriaQuery) throws HibernateException {

        StringBuffer result = new StringBuffer();
        Matcher matcher = paramPattern.matcher(sql);
        while (matcher.find()) {
            String token = matcher.group(1);
            String[] columns = criteriaQuery.getColumnsUsingProjection(criteria, token);
            if (columns.length > 0) {
                matcher.appendReplacement(result, columns[0]);
            }
        }
        matcher.appendTail(result);
        return result.toString();

    }

    @Override
    public TypedValue[] getTypedValues(Criteria criteria, CriteriaQuery criteriaQuery) throws HibernateException {
        return new TypedValue[0];
    }

    @Override
    public String toString() {
        return sql;
    }

    protected SQLParamRestriction(String sql) {
        this.sql = sql;
    }

    public static Criterion restriction(String sql) {
        return new SQLParamRestriction(sql);
    }

}