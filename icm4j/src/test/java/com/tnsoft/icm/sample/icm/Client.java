package com.tnsoft.icm.sample.icm;

import com.tnsoft.icm.icm4j.ServiceFactory;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.icm.service.MyHello;
import com.tnsoft.icm.util.PoolProviderFactory;

public class Client implements SampleCase {

	public static void main(String[] args) {
		ServiceFactory factory = ServiceFactory.newInstance();
		factory.setPoolProvider(PoolProviderFactory.newDequeChannelPoolProvider());
		MyHello myHello = factory.getService(MyHello.class, HOST, RPC_PORT);
		Out<Long> v = new Out<Long>();
		String result = myHello.sayHello("[From java]: Hello cpp!", (short) 3000, v);
		System.out.println("---------------------------------------------------------");
		System.out.printf("result=%s\n", result);
		System.out.printf("v=%d\n", v.get());
		System.out.println("---------------------------------------------------------");
	}

}
