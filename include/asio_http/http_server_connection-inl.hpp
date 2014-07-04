#if !defined(ASIO_HTTP_HTTP_SERVER_CONNECTION_H_INCLUDED_)
#error "Invalid include order"
#endif

template <typename SocketType>
basic_http_connection<SocketType>::basic_http_connection(boost::asio::io_service& io_service)
	: socket_(io_service)
	, parser_()
{
	// Initialize parser
	http_parser_init(&parser_, HTTP_REQUEST);
	parser_.data = this;
	std::memset(&settings_, 0, sizeof(settings_));
	settings_.on_url = &basic_http_connection::on_url;
	settings_.on_message_begin = &basic_http_connection::on_message_begin;
	settings_.on_status_complete = &basic_http_connection::on_status_complete;
	settings_.on_header_field = &basic_http_connection::on_header_field;
	settings_.on_header_value = &basic_http_connection::on_header_value;
	settings_.on_headers_complete = &basic_http_connection::on_headers_complete;
	settings_.on_body = &basic_http_connection::on_body;
	settings_.on_message_complete = &basic_http_connection::on_message_complete;
}

template <typename SocketType>
void basic_http_connection<SocketType>::start()
{
	boost::asio::async_read(socket_, buffer_, boost::asio::transfer_at_least(64),
	  boost::bind(&basic_http_connection<SocketType>::handler,
			this->shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

template <typename SocketType>
int basic_http_connection<SocketType>::on_message_begin(http_parser * parser)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	std::cout << "complete" << std::endl;
	return 0;
}
template <typename SocketType>
int basic_http_connection<SocketType>::on_status_complete(http_parser * parser)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	return 0;
}
template <typename SocketType>
int basic_http_connection<SocketType>::on_header_field(http_parser * parser, const char * at, size_t length)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	return 0;
}
template <typename SocketType>
int basic_http_connection<SocketType>::on_header_value(http_parser * parser, const char * at, size_t length)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	return 0;
}
template <typename SocketType>
int basic_http_connection<SocketType>::on_headers_complete(http_parser * parser)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	return 0;
}
template <typename SocketType>
int basic_http_connection<SocketType>::on_body(http_parser * parser, const char * at, size_t length)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	return 0;
}

template <typename SocketType>
int basic_http_connection<SocketType>::on_url(http_parser* parser, const char *at, size_t length)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	conn->request_url_.append(at, at + length);
	return 0;
}

template <typename SocketType>
int basic_http_connection<SocketType>::on_message_complete(http_parser * parser)
{
	basic_http_connection * conn = static_cast<basic_http_connection *>(parser->data);
	std::cout << "message complete" << std::endl;
	return 0;
}

template <typename SocketType>
void basic_http_connection<SocketType>::handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	std::cout << "bytes bytes_transferred = " << bytes_transferred << std::endl;
	
	if (!error && bytes_transferred)
	{
		const char * data = boost::asio::buffer_cast<const char *>(buffer_.data());
		std::size_t nsize = http_parser_execute(&parser_, &settings_, data, bytes_transferred);
		std::cout << "nsize = " << nsize << std::endl;
		if (nsize != bytes_transferred)
		{
			std::cout << "http parser execute fail " << nsize << "/" << bytes_transferred << std::endl;
			socket_.close();
			return;
		}
		buffer_.consume(nsize);
		int is_final = http_body_is_final(&parser_);
		int is_keep_alive = http_should_keep_alive(&parser_);
		std::cout << "consumed " << nsize << " bytes" <<std::endl;
		start();
	}
	else
	{
		socket_.close();
	}
}

template <typename SocketType>
void basic_http_connection<SocketType>::handle_write(const boost::system::error_code& /*error*/,
	size_t /*bytes_transferred*/)
{
	
}

template <typename SocketType>
void basic_http_connection<SocketType>::send_response(std::string message)
{
	message.append("\r\n\r\n");
	boost::asio::async_write(socket_, boost::asio::buffer(message),
		boost::bind(&basic_http_connection<SocketType>::handle_write, this->shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}