//
//  test-all.cpp
//  mailcore2
//
//  Created by DINH Viêt Hoà on 3/29/13.
//  Copyright (c) 2013 MailCore. All rights reserved.
//

#include "test-all.h"

#include <MailCore/MailCore.h>
#if __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif
#if __linux__ && !defined(ANDROID) && !defined(__ANDROID__)
#include <glib.h>
#endif
#ifdef _MSC_VER
#include <Windows.h>
#endif


#ifdef _MSC_VER
#include "unistd.h"
#else
#include <unistd.h>
#endif


//extern "C" {
//	extern int mailstream_debug;
//}

static mailcore::String * password = NULL;
static mailcore::String * displayName = NULL;
static mailcore::String * email = NULL;
#if __linux__ && !defined(ANDROID) && !defined(__ANDROID__)
static GMainLoop * s_main_loop = NULL;
#endif

#ifdef _MSC_VER
static void win32MainLoop(void)
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
#endif

static void mainLoop(void)
{
#if __APPLE__
	CFRunLoopRun();
#elif __linux__ && !defined(ANDROID) && !defined(__ANDROID__)
	g_main_loop_run(s_main_loop);
#elif defined(_MSC_VER)
	win32MainLoop();
#endif
}

class TestOperation : public mailcore::Operation {
    void main()
    {
        MCLog("coin %p", this);
    }
};

class TestCallback : public mailcore::Object, public mailcore::OperationCallback {
    virtual void operationFinished(mailcore::Operation * op)
    {
        MCLog("callback coin %p %p %s", this, op, MCUTF8DESC(this));
    }
};

static mailcore::String * temporaryFilenameForTest()
{
	char tempfile[] = "C:\\tmp\\mailcore2-test-XXXXXX";
    char * result = mktemp(tempfile);
    if (result == NULL) {
        return NULL;
    }
    return mailcore::String::stringWithFileSystemRepresentation(tempfile);
}

static mailcore::Data * testMessageBuilder()
{
    mailcore::Address * address = new mailcore::Address();
    address->setDisplayName(displayName);
    address->setMailbox(email);
    
    address->release();
    
    mailcore::MessageBuilder * msg = new mailcore::MessageBuilder();
    
    msg->header()->setFrom(mailcore::Address::addressWithDisplayName(displayName, email));
    mailcore::Array * to = new mailcore::Array();
    mailcore::Array * bcc = new mailcore::Array();
    to->addObject(mailcore::Address::addressWithDisplayName(MCSTR("126_firemail"), MCSTR("firemail_wang@126.com")));
    to->addObject(mailcore::Address::addressWithDisplayName(MCSTR("163_9857"), MCSTR("15313159857@163.com")));
    bcc->addObject(mailcore::Address::addressWithDisplayName(MCSTR("189_9857_hide"), MCSTR("15313159857@189.cn")));
    msg->header()->setTo(to);
    msg->header()->setBcc(bcc);
    to->release();
    bcc->release();
    MCAssert(msg->header()->allExtraHeadersNames()->count() == 0);
    msg->header()->setExtraHeader(MCSTR("X-Custom-Header"), MCSTR("Custom Header Value"));
    msg->header()->setSubject(MCSTR("Mon projet d'été"));
    msg->setHTMLBody(MCSTR("<div>Hello <img src=\"cid:1234\"></div>"));
    msg->addAttachment(mailcore::Attachment::attachmentWithContentsOfFile(MCSTR("first-filename")));
    msg->addAttachment(mailcore::Attachment::attachmentWithContentsOfFile(MCSTR("second-filename")));
    mailcore::Attachment * attachment = mailcore::Attachment::attachmentWithContentsOfFile(MCSTR("third-image-attachment"));
    attachment->setContentID(MCSTR("1234"));
    msg->addRelatedAttachment(attachment);
    
    mailcore::Data * data = msg->data();
    
    MCLog("%s", data->bytes());

    mailcore::String *filename = temporaryFilenameForTest();
    msg->writeToFile(filename);
    mailcore::Data *fileData = mailcore::Data::dataWithContentsOfFile(filename);
    MCAssert(data->isEqual(fileData));

    mailcore::MessageBuilder * msg2 = new mailcore::MessageBuilder(msg);
    mailcore::String *htmlBody = msg->htmlBody();
    mailcore::String *htmlBody2 = msg2->htmlBody();
    MCAssert(htmlBody->isEqual(htmlBody2));
    
    msg->release();
    msg2->release();
    unlink(filename->fileSystemRepresentation());

    return data;
}

static mailcore::Data * testMessageBuilder2()
{
	mailcore::Address * address = new mailcore::Address();
	address->setDisplayName(displayName);
	address->setMailbox(email);

	address->release();

	mailcore::MessageBuilder * msg = new mailcore::MessageBuilder();

	msg->header()->setFrom(mailcore::Address::addressWithDisplayName(displayName, email));
	mailcore::Array * to = new mailcore::Array();
	to->addObject(mailcore::Address::addressWithDisplayName(MCSTR("126_firemail"), MCSTR("firemail_wang@126.com")));
	msg->header()->setTo(to);
	to->release();
	MCAssert(msg->header()->allExtraHeadersNames()->count() == 0);
	
	msg->header()->setSubject(MCSTR("HelloFrom mailcore"));
	msg->setHTMLBody(MCSTR("<div>Hello mailcore</div>"));
	mailcore::Data * data = msg->data();

	MCLog("%s", data->bytes());

	mailcore::String *filename = temporaryFilenameForTest();
	msg->writeToFile(filename);
	mailcore::Data *fileData = mailcore::Data::dataWithContentsOfFile(filename);
	MCAssert(data->isEqual(fileData));

	mailcore::MessageBuilder * msg2 = new mailcore::MessageBuilder(msg);
	mailcore::String *htmlBody = msg->htmlBody();
	mailcore::String *htmlBody2 = msg2->htmlBody();
	MCAssert(htmlBody->isEqual(htmlBody2));

	msg->release();
	msg2->release();
	unlink(filename->fileSystemRepresentation());

	return data;
}

static void testMessageParser(mailcore::Data * data)
{
    mailcore::MessageParser * parser = mailcore::MessageParser::messageParserWithData(data);
    MCLog("%s", MCUTF8DESC(parser));
    MCLog("HTML rendering");
    MCLog("%s", MCUTF8(parser->htmlRendering())); //没测试通过  Microsoft C++ exception: std::bad_alloc at memory location 0x002BE55C.
    //MCLog("%s", MCUTF8(parser->plainTextBodyRendering(true)));//没测试通过
    //MCLog("%s", MCUTF8(parser->plainTextBodyRendering(false))); //没测试通过
}

static void testIMAP_GMAIL()
{
	email = MCSTR("hcj@gmail.com");
	password = MCSTR("hc");
	displayName = MCSTR("hcj");

	mailcore::IMAPSession* session;
	mailcore::ErrorCode error;

	session = new mailcore::IMAPSession();
	session->setHostname(MCSTR("imap.gmail.com")); //imap.googlemail.com imap.gmail.com
	session->setPort(993);
	session->setUsername(email);
	session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeTLS);
	session->setOAuth2Token(MCSTR("ya29.a0AfH6SMCxVYTpn8D7EvWdFAdyYIMtjBX9oZnVARgsLZswF2S5z5vf1pMPYHYZWFK6pY64zJhmrswAYoIMECtjH4mQOd66jE_9K1cZuX3Rt3KYsuzdgeQlDv2FX-Lr58peaOWdMgVD-kUgZymmgGISEjUDnvWQo5PRKfE"));
	session->setAuthType(mailcore::AuthTypeXOAuth2);

	session->connect(&error);
	session->login(&error);
	session->select(MCSTR("INBOX"), &error);
	mailcore::IMAPFolderStatus* fs = session->folderStatus(MCSTR("INBOX"), &error);
	MCLog("%d", fs->unseenCount());

	session->release();

}

static void testIMAP_139()
{
	email = MCSTR("153135557@139.com");
	password = MCSTR("98594");
	displayName = MCSTR("139_9857");

	mailcore::IMAPSession* session;
	mailcore::ErrorCode error;

	session = new mailcore::IMAPSession();
	session->setHostname(MCSTR("imap.139.com"));
	session->setPort(143);
	session->setUsername(email);
	session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeClear);
	//选择邮件夹
    session->connect(&error);
    session->login(&error);
	session->select(MCSTR("INBOX"), &error);
	mailcore::IMAPFolderStatus* fs = session->folderStatus(MCSTR("INBOX"), &error);
    MCLog("%d", fs->unseenCount());
	mailcore::Array* folders = session->fetchAllFolders(&error);
   //MCLog("%s", MCUTF8DESC(folders));

	session->release();

}

static void testIMAP()
{
    mailcore::IMAPSession * session;
    mailcore::ErrorCode error;
    
    session = new mailcore::IMAPSession();
    session->setHostname(MCSTR("imap.139.com"));
    session->setPort(143);
    session->setUsername(email);
    session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeClear);
    
    mailcore::IMAPMessagesRequestKind requestKind = (mailcore::IMAPMessagesRequestKind)
    (mailcore::IMAPMessagesRequestKindHeaders | mailcore::IMAPMessagesRequestKindStructure |
     mailcore::IMAPMessagesRequestKindInternalDate | mailcore::IMAPMessagesRequestKindHeaderSubject |
     mailcore::IMAPMessagesRequestKindFlags);
    //mailcore::Array * messages = session->fetchMessagesByUID(MCSTR("INBOX"),
    //                                                         requestKind, mailcore::IndexSet::indexSetWithRange(mailcore::RangeMake(1, UINT64_MAX)), NULL, &error);
	mailcore::Array* messages = session->fetchMessagesByNumber(MCSTR("INBOX"),
		requestKind, mailcore::IndexSet::indexSetWithRange(mailcore::RangeMake(0, 3)), NULL, &error);
    
    MCLog("%s", MCUTF8DESC(messages));
    
    session->release();
}

static void testIMAPMove()
{
    mailcore::IMAPSession * session;
    mailcore::HashMap *uidMapping;
    mailcore::ErrorCode error;
    
    session = new mailcore::IMAPSession();
    session->setHostname(MCSTR("imap.139.com"));
    session->setPort(143);
    session->setUsername(email);
    session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeClear);

    session->moveMessages(MCSTR("INBOX"),
                          mailcore::IndexSet::indexSetWithIndex(14990),
                          MCSTR("Personal"), &uidMapping, &error);
    
    MCLog("mapping: %s, error: %i", MCUTF8DESC(uidMapping), error);
    
    session->release();
}

static void testIMAPCapability()
{
    mailcore::IMAPSession * session;
    mailcore::ErrorCode error;

    session = new mailcore::IMAPSession();
    session->setHostname(MCSTR("imap.139.com"));
    session->setPort(143);
    session->setUsername(email);
    session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeClear);

    mailcore::IndexSet *caps = session->capability(&error);

    MCLog("capability: %s, error: %i", MCUTF8DESC(caps), error);

    session->release();
}

static void testSMTP(mailcore::Data * data)
{
    mailcore::SMTPSession * smtp;
    mailcore::ErrorCode error;
    
    smtp = new mailcore::SMTPSession();
    
    smtp->setHostname(MCSTR("smtp.139.com"));
    smtp->setPort(25);
    smtp->setUsername(email);
    smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);
    
    smtp->sendMessage(data, NULL, &error);
    
    smtp->release();
}

static void parseAddressesFromRfc822(mailcore::String * filename, mailcore::Array ** pRecipients, mailcore::Address ** pFrom)
{
    mailcore::MessageParser * parser = mailcore::MessageParser::messageParserWithContentsOfFile(filename);

    mailcore::Array * recipients = mailcore::Array::array();
    if (parser->header()->to() != NULL) {
        recipients->addObjectsFromArray(parser->header()->to());
    }
    if (parser->header()->cc() != NULL) {
        recipients->addObjectsFromArray(parser->header()->cc());
    }
    if (parser->header()->bcc() != NULL) {
        recipients->addObjectsFromArray(parser->header()->bcc());
    }
    *pRecipients = recipients;
    *pFrom = parser->header()->from();
}

static void testSendingMessageFromFileViaSMTP(mailcore::Data * data)
{
    mailcore::SMTPSession * smtp;
    mailcore::ErrorCode error;

    mailcore::String * filename = temporaryFilenameForTest();
    data->writeToFile(filename);

    smtp = new mailcore::SMTPSession();

    smtp->setHostname(MCSTR("smtp.139.com"));
    smtp->setPort(25);
    smtp->setUsername(email);
    smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);

    mailcore::Array * recipients;
    mailcore::Address * from;
    parseAddressesFromRfc822(filename, &recipients, &from);

    smtp->sendMessage(from, recipients, filename, NULL, &error);

    smtp->release();
}

static void testSendingMessageFromFileViaSMTP()
{
	mailcore::SMTPSession * smtp;
	mailcore::ErrorCode error;

	char tempfile[] = "C:\\tmp\\mailcore2-test-a08400";
	mailcore::String * filename = mailcore::String::stringWithFileSystemRepresentation(tempfile);
	smtp = new mailcore::SMTPSession();

	smtp->setHostname(MCSTR("smtp.139.com"));
	smtp->setPort(25);
	smtp->setUsername(email);
	smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);

	mailcore::Array * recipients;
	mailcore::Address * from;
	parseAddressesFromRfc822(filename, &recipients, &from);

	smtp->sendMessage(from, recipients, filename, NULL, &error);

	smtp->release();
}

static void testPOP()
{
    mailcore::POPSession * session;
    mailcore::ErrorCode error;
    
    session = new mailcore::POPSession();
    session->setHostname(MCSTR("pop.gmail.com"));
    session->setPort(995);
    session->setUsername(email);
    session->setPassword(password);
    session->setConnectionType(mailcore::ConnectionTypeTLS);
    
    mailcore::Array * messages = session->fetchMessages(&error);
    MCLog("%s", MCUTF8DESC(messages));
    
    session->release();
}

static void testNNTP()
{
    mailcore::NNTPSession * session;
    mailcore::ErrorCode error;
    
    session = new mailcore::NNTPSession();
    session->setHostname(MCSTR("news.gmane.org."));
    session->setPort(119);
//    session->setUsername(email);
//    session->setPassword(password);
    session->setConnectionType(mailcore::ConnectionTypeClear);
    
    session->checkAccount(&error);
    mailcore::Array * messages = session->listAllNewsgroups(&error);
    MCLog("%s", MCUTF8DESC(messages));
    
    session->release();

}

static void testOperationQueue()
{
    mailcore::OperationQueue * queue = new mailcore::OperationQueue();
    
    TestCallback * callback = new TestCallback();

    for(unsigned int i = 0 ; i < 100 ; i ++) {
        mailcore::Operation * op = new TestOperation();
        op->setCallback(callback);
        queue->addOperation(op);
        op->release();
    }
    
	mainLoop();

    queue->release();
}

class TestSMTPCallback : public mailcore::Object, public mailcore::OperationCallback, public mailcore::SMTPOperationCallback {
    virtual void operationFinished(mailcore::Operation * op)
    {
        MCLog("callback %s %s", MCUTF8DESC(op), MCUTF8DESC(this));
    }
    
    virtual void bodyProgress(mailcore::SMTPOperation * op, unsigned int current, unsigned int maximum)
    {
        MCLog("progress %s %s %i/%i", MCUTF8DESC(op), MCUTF8DESC(this), current, maximum);
    }
};

static void testAsyncSMTP(mailcore::Data * data)
{
    mailcore::SMTPAsyncSession * smtp;
    TestSMTPCallback * callback = new TestSMTPCallback();
    
    smtp = new mailcore::SMTPAsyncSession();
    
    smtp->setHostname(MCSTR("smtp.139.com"));
    smtp->setPort(25);
    smtp->setUsername(email);
    smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);
    
    mailcore::SMTPOperation * op = smtp->sendMessageOperation(data);
    op->setSmtpCallback(callback);
    op->setCallback(callback);
    op->start();
    
	mainLoop();

    //smtp->release();
}

static void testAsyncSendMessageFromFileViaSMTP(mailcore::Data * data)
{
    mailcore::SMTPAsyncSession * smtp;
    TestSMTPCallback * callback = new TestSMTPCallback();

    mailcore::String * filename = temporaryFilenameForTest();
    data->writeToFile(filename);

    mailcore::Array * recipients;
    mailcore::Address * from;
    parseAddressesFromRfc822(filename, &recipients, &from);

    smtp = new mailcore::SMTPAsyncSession();

    smtp->setHostname(MCSTR("smtp.139.com"));
    smtp->setPort(25);
    smtp->setUsername(email);
    smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);

    mailcore::SMTPOperation * op = smtp->sendMessageOperation(from, recipients, filename);
    op->setSmtpCallback(callback);
    op->setCallback(callback);
    op->start();

    mainLoop();

    //smtp->release();
}


static void testAsyncSendMessageFromFileViaSMTP()
{
	mailcore::SMTPAsyncSession * smtp;
	TestSMTPCallback * callback = new TestSMTPCallback();

	char tempfile[] = "C:\\tmp\\mailcore2-test-a08400";
	mailcore::String * filename = mailcore::String::stringWithFileSystemRepresentation(tempfile);

	mailcore::Array * recipients;
	mailcore::Address * from;
	parseAddressesFromRfc822(filename, &recipients, &from);

	smtp = new mailcore::SMTPAsyncSession();

	smtp->setHostname(MCSTR("smtp.139.com"));
	smtp->setPort(25);
	smtp->setUsername(email);
	smtp->setPassword(password);
	smtp->setConnectionType(mailcore::ConnectionTypeClear);

	mailcore::SMTPOperation * op = smtp->sendMessageOperation(from, recipients, filename);
	op->setSmtpCallback(callback);
	op->setCallback(callback);
	op->start();

	mainLoop();

	//smtp->release();
}

class TestIMAPCallback : public mailcore::Object, public mailcore::OperationCallback, public mailcore::IMAPOperationCallback {
    virtual void operationFinished(mailcore::Operation * op)
    {
        mailcore::IMAPFetchMessagesOperation * fetchOp = (mailcore::IMAPFetchMessagesOperation *) op;
        (void) (fetchOp);
        //MCLog("callback %s %s %s", MCUTF8DESC(op), MCUTF8DESC(fetchOp->messages()), MCUTF8DESC(this));
    }
    
    virtual void bodyProgress(mailcore::IMAPOperation * op, unsigned int current, unsigned int maximum)
    {
        MCLog("progress %s %s %i/%i", MCUTF8DESC(op), MCUTF8DESC(this), current, maximum);
    }
    
    virtual void itemProgress(mailcore::IMAPOperation * op, unsigned int current, unsigned int maximum)
    {
        MCLog("item progress %s %s %i/%i", MCUTF8DESC(op), MCUTF8DESC(this), current, maximum);
    }
};

static void testAsyncIMAP()
{
    mailcore::IMAPAsyncSession * session;
    TestIMAPCallback * callback = new TestIMAPCallback();
    
    session = new mailcore::IMAPAsyncSession();
    session->setHostname(MCSTR("imap.139.com"));
    session->setPort(143);
    session->setUsername(email);
    session->setPassword(password);
	session->setConnectionType(mailcore::ConnectionTypeClear);
    
    mailcore::IMAPMessagesRequestKind requestKind = (mailcore::IMAPMessagesRequestKind)
    (mailcore::IMAPMessagesRequestKindHeaders | mailcore::IMAPMessagesRequestKindStructure |
     mailcore::IMAPMessagesRequestKindInternalDate | mailcore::IMAPMessagesRequestKindHeaderSubject |
     mailcore::IMAPMessagesRequestKindFlags);
    mailcore::IMAPFetchMessagesOperation * op = session->fetchMessagesByUIDOperation(MCSTR("INBOX"), requestKind, mailcore::IndexSet::indexSetWithRange(mailcore::RangeMake(1, UINT64_MAX)));
    op->setCallback(callback);
    op->setImapCallback(callback);
    op->start();
    //MCLog("%s", MCUTF8DESC(messages));
	mainLoop();

    //session->release();
}

class TestPOPCallback : public mailcore::Object, public mailcore::OperationCallback, public mailcore::POPOperationCallback {
    virtual void operationFinished(mailcore::Operation * op)
    {
        mailcore::POPFetchMessagesOperation * fetchOp = (mailcore::POPFetchMessagesOperation *) op;
        MCLog("callback %s %s", MCUTF8DESC(fetchOp->messages()), MCUTF8DESC(this));
    }
    
    virtual void bodyProgress(mailcore::IMAPOperation * op, unsigned int current, unsigned int maximum)
    {
        MCLog("progress %s %s %i/%i", MCUTF8DESC(op), MCUTF8DESC(this), current, maximum);
    }
};

static void testAsyncPOP()
{
    mailcore::POPAsyncSession * session;
    TestPOPCallback * callback = new TestPOPCallback();
    
    session = new mailcore::POPAsyncSession();
    session->setHostname(MCSTR("pop.gmail.com"));
    session->setPort(995);
    session->setUsername(email);
    session->setPassword(password);
    session->setConnectionType(mailcore::ConnectionTypeTLS);
    
    mailcore::POPFetchMessagesOperation * op = session->fetchMessagesOperation();
    op->setCallback(callback);
    op->setPopCallback(callback);
    op->start();
    
    //mailcore::Array * messages = session->fetchMessages(&error);
    //MCLog("%s", MCUTF8DESC(messages));
    
	mainLoop();
}

static void testAddresses()
{
    mailcore::Address *addr = mailcore::Address::addressWithNonEncodedRFC822String(MCSTR("DINH Viêt Hoà <hoa@etpan.org>"));
    MCLog("%s %s", MCUTF8DESC(addr->nonEncodedRFC822String()), MCUTF8DESC(addr->RFC822String()));
    
    mailcore::Array *addresses = mailcore::Address::addressesWithNonEncodedRFC822String(MCSTR("My Email1 <email1@gmail.com>, DINH Viêt Hoà <hoa@etpan.org>,\"Email3, My\" <my.email@gmail.com>"));
    MCLog("%s", MCUTF8DESC(addresses));
    mailcore::String *str = mailcore::Address::nonEncodedRFC822StringForAddresses(addresses);
    MCLog("%s", MCUTF8DESC(str));
    str = mailcore::Address::RFC822StringForAddresses(addresses);
    MCLog("%s", MCUTF8DESC(str));
}

static void testAttachments()
{
    mailcore::Attachment *attachment = mailcore::Attachment::attachmentWithText(MCSTR("Hello World"));
    attachment->setCharset(NULL);
    mailcore::String * str = attachment->decodedString();
    MCLog("%s", MCUTF8DESC(str));
}

void testAll()
{
    
    mailcore::setICUDataDirectory(MCSTR("/usr/local/share/icu"));
    
    email = MCSTR("15313159857@139.com");
    password = MCSTR("xxx");
    displayName = MCSTR("139_9857");
    
#if __linux__ && !defined(ANDROID) && !defined(__ANDROID__)
    s_main_loop = g_main_loop_new (NULL, FALSE);
#endif
    
    mailcore::AutoreleasePool * pool = new mailcore::AutoreleasePool();
    MCLogEnabled = 1;
    //mailstream_debug = 1;
    //testIMAP_GMAIL();
    //testIMAP_139();
    //MCLog("test");
    
    
    //mailcore::Data * data = testMessageBuilder();
	//mailcore::Data * data = testMessageBuilder2(); //日期时间 的时区不对  Date: Sun, 2 Feb 2020 21:40:33 +0000
    //testMessageParser(data);
    //testSMTP(data);
	//testSendingMessageFromFileViaSMTP();  //ok
    //testSendingMessageFromFileViaSMTP(data);
    //testIMAP(); //ok
    //testIMAPMove();
    //testIMAPCapability();
    //testPOP();
    //testNNTP();
    //testAsyncSMTP(data);
   //testAsyncSendMessageFromFileViaSMTP(data);
   //testAsyncSendMessageFromFileViaSMTP(); //ok
    //testAsyncIMAP(); //ok
    //testAsyncPOP();
    //testAddresses(); //ok
    //testAttachments(); //ok

    //char* subject = "=?utf-8?B?5ZKM6YCa6KGM6K+BLeWvhueggeWPmOabtA==?=";
    //mailcore::String strSub = mailcore::String::stringByDecodingMIMEHeaderValue(subject);


    pool->release();
}
