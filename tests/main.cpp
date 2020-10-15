//
//  main.m
//  tests
//
//  Created by DINH Viêt Hoà on 1/10/13.
//  Copyright (c) 2013 MailCore. All rights reserved.
//

#include "test-all.h"
#if __APPLE__
#include "test-all-mac.h"
#endif
// --mode migrate
int main(int argc, const char * argv[])
{
    //fprintf(stdout, "%s", "test1");
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i++], "--mode")) {
			if (!strcmp(argv[i], "migrate")) {
                char* data = "{\"status\":\"running migration\"}";
                fprintf(stdout, "%s", data);
                //_sleep(3000);
                //fprintf(stdout, "%s\n", "running vacuum");
			}
			if (!strcmp(argv[i], "test")) {
                char* data = "{\"id\":\"b800a344\", \"metadata\" : [] , \"name\" : \"15313159857\", \"provider\" : \"imap\", \"emailAddress\" : \"15313159857@139.com\", \"settings\" : {\"imap_host\":\"imap.139.com\", \"imap_port\" : 143, \"imap_username\" : \"15313159857@139.com\", \"imap_password\" : \"9857139i94\", \"imap_security\" : \"none\", \"imap_allow_insecure_ssl\" : false, \"smtp_host\" : \"smtp.139.com\", \"smtp_port\" : 25, \"smtp_username\" : \"15313159857@139.com\", \"smtp_password\" : \"9857139i94\", \"smtp_security\" : \"none\", \"smtp_allow_insecure_ssl\" : false}, \"label\" : \"15313159857@139.com\", \"autoaddress\" : {\"type\":\"bcc\", \"value\" : \"\"}, \"aliases\" : [] , \"syncState\" : \"ok\", \"authedAt\" : 1602299324.14, \"__cls\" : \"Account\"}";
				fprintf(stdout, "%s", data);
			}
			if (!strcmp(argv[i], "sync")) {
				char* data = "{\"modelClass\":\"Folder\",\"modelJSONs\":[{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"Lk12eu8qWXuhFfhNgWQL95A9kY2aTz54ARP64zovM\",\"path\":\"INBOX\",\"role\":\"inbox\",\"v\":2},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"StmzU7PDLSyVMtFArwBZMDv5EbSv45TjmXjbaPZCV\",\"path\":\"&g0l6P3ux-\",\"role\":\"drafts\",\"v\":2},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"Jpge8BvRtPRvkZTJHhSFqPBKhnaT2DyXQRGuNN98R\",\"path\":\"&XfJT0ZAB-\",\"role\":\"sent\",\"v\":2},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"q2zZWGt1Ky4YEkam5YokAALNktbtxA1xmPVB84BDu\",\"path\":\"&XfJSIJZk-\",\"role\":\"trash\",\"v\":2},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"d6Pqqtiq83Wn94G8GUvpujjWQ6bcYoPjUDZyRkGKH\",\"path\":\"&V4NXPpCuTvY-\",\"role\":\"spam\",\"v\":2},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"CtDtb7hksdT1NNG1kBYvcN8UJRNTUrVBWDHCzXEdr\",\"path\":\"&UWxT+JCuTvY-\",\"role\":\"\",\"v\":1},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"V9AQF7D63q3qKCS6ZzJA1kfC3AtzeMWbohkvuRdRb\",\"path\":\"Personal\",\"role\":\"\",\"v\":1},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"TeNLJcSZPWm9t1RZ5dFBybQAt5RFrqLRbcEsf3XjE\",\"path\":\"Mailspring\",\"role\":\"\",\"v\":1},{\"__cls\":\"Folder\",\"aid\":\"b800a344\",\"id\":\"xxnLufF2J6YeBsBnXB8RX1UXbeY5pcjbbMGi3RCz\",\"path\":\"Mailspring/Snoozed\",\"role\":\"snoozed\",\"v\":2}],\"type\":\"persist\"}\r";
				fprintf(stdout, "%s", data);
			}
        }
    }

//    
//        
    //testAll();
#if __APPLE__
    testAllMac();
#endif
    return 0;
}

