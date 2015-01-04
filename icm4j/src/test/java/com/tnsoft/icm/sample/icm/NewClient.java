package com.tnsoft.icm.sample.icm;

import java.nio.ByteOrder;

import com.tnsoft.icm.icm4j.IcmTcpClient;
import com.tnsoft.icm.icm4j.PooledSocketChannelProvider;
import com.tnsoft.icm.icm4j.protocol.extension.Out;
import com.tnsoft.icm.sample.SampleCase;
import com.tnsoft.icm.sample.icm.service.MyHello;

public class NewClient implements SampleCase {

	public static void main(String[] args) {
		IcmTcpClient client = new IcmTcpClient();
		// Default order is also BIG_ENDIAN.
		client.setByteOrder(ByteOrder.BIG_ENDIAN);
//		client.setSocketProvider(new PooledSocketProvider());
		// Default pool max size is also 3.
		client.setSocketChannelProvider(new PooledSocketChannelProvider(3));
		MyHello myHello = client.getService(MyHello.class, HOST, RPC_PORT);
		Out<Long> v = new Out<Long>();
		String result = myHello.sayHello("[From java]: Hello cpp!", (short) 3000, v);
		System.out.println("---------------------------------------------------------");
		System.out.printf("result=%s\n", result);
		System.out.printf("v=%d\n", v.get());
		System.out.println("---------------------------------------------------------");
	}

}
