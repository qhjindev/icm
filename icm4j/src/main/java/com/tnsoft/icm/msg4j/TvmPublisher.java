package com.tnsoft.icm.msg4j;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.HashSet;
import java.util.Set;

import com.tnsoft.icm.icm4j.protocol.MessageHead;
import com.tnsoft.icm.msg4j.protocol.Msg;
import com.tnsoft.icm.msg4j.protocol.MsgResponse;
import com.tnsoft.icm.msg4j.protocol.Publish;
import com.tnsoft.icm.msg4j.protocol.TvmEvent;
import com.tnsoft.icm.util.BufferFactory;

public class TvmPublisher extends MsgClient {

	private Set<String> publishedTopicIdSet = new HashSet<String>();

	public TvmPublisher(String hostname, int port) {
		super(hostname, port);
	}

	public TvmPublisher(String hostname) {
		this(hostname, 10001);
	}

	public synchronized void openAndConnect() throws IOException {
		if (socketChannel == null || !socketChannel.isOpen()) {
			socketChannel = SocketChannel.open();
		}
		while (!socketChannel.isConnected()) {
			if (socketChannel.isConnectionPending()) {
				socketChannel.finishConnect();
			} else {
				socketChannel.connect(new InetSocketAddress(hostname, port));
			}
		}
	}

	public synchronized void close() throws IOException {
		log.debug("Closing the Publisher...");
		socketChannel.close();
		log.info("The Publisher has been closed.");
	}

	public synchronized boolean publish(String topicId) throws IOException {
		log.info("Publish a topic with ID [%s].", topicId);
		Publish publish = new Publish(topicId);
		publish.writeBuffer(writer);
		log.debug("Writing publish [%s] on socket channel...", topicId);
		socketChannel.write(publish.getBuffer());
		log.debug("Writing publish [%s] finished.", topicId);
		log.debug("Reading publish [%s] response on socket channel...", topicId);
		ByteBuffer buffer = BufferFactory.getInstance().getBuffer(MessageHead.MESSAGE_HEAD_SIZE);
		log.debug("Reading response head...");
		channelRead(socketChannel, buffer);
		buffer.flip();
		int size = reader.readInt(buffer);
		int type = reader.readByte(buffer);
		log.debug("Reading response head finished, size=[%d], type=[%d]", size, type);
		buffer = BufferFactory.getInstance().getBuffer(size);
		log.debug("Reading response body...");
		channelRead(socketChannel, buffer);
		buffer.flip();
		log.debug("Reading response body finished.");
		switch (type) {
		case Msg.TYPE_OK:
			MsgResponse response = new MsgResponse(buffer);
			response.readTopicId(reader);
			log.info("The topic with ID [%s] has published.", response.getTopicId());
			publishedTopicIdSet.add(response.getTopicId());
			return true;
		case Msg.TYPE_UNKNOW:
		default:
			log.error("Get an unknown response type when publishing the topic with ID [%s].", topicId);
			return false;
		}
	}

	public synchronized boolean unpublish(String topicId) throws IOException {
		log.info("Unpublish a topic with ID [%s].", topicId);
		Publish publish = new Publish(false, topicId);
		publish.writeBuffer(writer);
		log.debug("Writing un-publish [%s] on socket channel...", topicId);
		socketChannel.write(publish.getBuffer());
		log.debug("Writing un-publish [%s] finished.", topicId);
		log.debug("Reading un-publish [%s] response on socket channel...", topicId);
		ByteBuffer buffer = BufferFactory.getInstance().getBuffer(MessageHead.MESSAGE_HEAD_SIZE);
		log.debug("Reading response head...");
		channelRead(socketChannel, buffer);
		buffer.flip();
		int size = reader.readInt(buffer);
		int type = reader.readByte(buffer);
		log.debug("Reading response head finished, size=[%d], type=[%d]", size, type);
		buffer = BufferFactory.getInstance().getBuffer(size);
		log.debug("Reading response body...");
		channelRead(socketChannel, buffer);
		buffer.flip();
		log.debug("Reading response body finished.");
		switch (type) {
		case Msg.TYPE_OK:
			MsgResponse response = new MsgResponse(buffer);
			response.readTopicId(reader);
			log.info("Unpublishing the topic with ID [%s] succeed.", response.getTopicId());
			publishedTopicIdSet.remove(response.getTopicId());
			return true;
		case Msg.TYPE_UNKNOW:
		default:
			log.error("Get an unknown response type when unpublishing the topic with ID [%s].", topicId);
			return false;
		}
	}

	public synchronized void event(String topicId, Object event) throws IOException {
		log.info("Send an event for the topic with ID [%s].", topicId);
		if (publishedTopicIdSet.contains(topicId)) {
			TvmEvent evt = new TvmEvent(topicId, event);
			evt.writeBuffer(writer, 4 * 1024);
			log.debug("Writing event with topicId [%s] on socket channel...", topicId);
			socketChannel.write(evt.getBuffer());
			log.debug("Writing event with topicId [%s] finished.", topicId);
			log.info("The event for the topic with ID [%s] has been sent.", topicId);
		} else {
			log.warn("The topic with ID [%s] hasn't been published!", topicId);
		}
	}

}
