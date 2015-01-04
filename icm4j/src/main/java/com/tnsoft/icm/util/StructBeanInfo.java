package com.tnsoft.icm.util;

import java.beans.BeanInfo;
import java.beans.IntrospectionException;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.tnsoft.util.logging.Log;
import com.tnsoft.util.logging.LogFactory;

public class StructBeanInfo {

	private static final Log log = LogFactory.getFormatterLog(StructBeanInfo.class);

	private static void fields(Class<?> clazz, List<Field> fields) {
		Class<?> superClass = clazz.getSuperclass();
		if (superClass != Object.class) {
			fields(superClass, fields);
		}
		Field[] fs = clazz.getDeclaredFields();
		fields.addAll(Arrays.asList(fs));
	}
	
	private Class<?> type;
	private Property[] properties;

	public StructBeanInfo(Class<?> type) {
		this.type = type;
		List<Field> fields = new ArrayList<Field>();
		fields(type, fields);
		properties = new Property[fields.size()];
		Map<String, Integer> nameIndexMap = new HashMap<String, Integer>();
		int index = 0;
		for (Field field : fields) {
			properties[index] = new Property(field);
			nameIndexMap.put(field.getName(), index++);
		}
		try {
			BeanInfo beanInfo = Introspector.getBeanInfo(type, Object.class);
			PropertyDescriptor[] pds = beanInfo.getPropertyDescriptors();
			for (PropertyDescriptor pd : pds) {
				index = nameIndexMap.get(pd.getName());
				properties[index].setPropertyDescriptor(pd);
			}
		} catch (IntrospectionException e) {
			log.error(e);
			throw new RuntimeException(e);
		}
	}

	public Object newInstance() {
		try {
			return type.newInstance();
		} catch (InstantiationException e) {
			log.error(e);
			throw new RuntimeException(e);
		} catch (IllegalAccessException e) {
			log.error(e);
			throw new RuntimeException(e);
		}
	}

	public Class<?> getType() {
		return type;
	}

	public void setType(Class<?> type) {
		this.type = type;
	}

	public Property[] getProperties() {
		return properties;
	}

	public void setProperties(Property[] properties) {
		this.properties = properties;
	}

	public class Property {

		private String name;
		private Class<?> type;
		private Type genericType;
		private Method writeMethod;
		private Method readMethod;
		
		public Property(Field field) {
			setName(field.getName());
			setType(field.getType());
			setGenericType(field.getGenericType());
		}

		public String getName() {
			return name;
		}

		public void setName(String name) {
			this.name = name;
		}

		public Class<?> getType() {
			return type;
		}

		public void setType(Class<?> type) {
			this.type = type;
		}

		public Type getGenericType() {
			return genericType;
		}

		public void setGenericType(Type genericType) {
			this.genericType = genericType;
		}

		public void setPropertyDescriptor(PropertyDescriptor propertyDescriptor) {
			this.writeMethod = propertyDescriptor.getWriteMethod();
			this.readMethod = propertyDescriptor.getReadMethod();
		}

		public void setValue(Object obj, Object value) {
			try {
				writeMethod.invoke(obj, value);
			} catch (IllegalArgumentException e) {
				log.error(e);
				throw new RuntimeException(e);
			} catch (IllegalAccessException e) {
				log.error(e);
				throw new RuntimeException(e);
			} catch (InvocationTargetException e) {
				log.error(e);
				throw new RuntimeException(e);
			}
		}

		public Object getValue(Object obj) {
			try {
				return readMethod.invoke(obj);
			} catch (IllegalArgumentException e) {
				log.error(e);
				throw new RuntimeException(e);
			} catch (IllegalAccessException e) {
				log.error(e);
				throw new RuntimeException(e);
			} catch (InvocationTargetException e) {
				log.error(e);
				throw new RuntimeException(e);
			}
		}

	}
}
