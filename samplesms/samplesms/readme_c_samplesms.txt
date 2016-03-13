Readme for c_samplesms, version 1.0
September, 2009
Copyright (c) 2009 QUALCOMM Incorporated. All rights reserved.
------------------------------------------------------------------


This sample application is to illustrate sending, receiving, and storing SMS/EMS messages on a BREW device.

With limitation on the number of bytes that can be carried in a message, 
application will perform message segmentation if the composed message is more than one segment long, 
and send each segment with proper UDH (User Data Header) concatenation information.

This sample application also demonstrates ISMS support to include media content in a message.
As limitation of this sample application, currently it only allows user to include one media content in a message,
either a small picture or a large picture.

Any sent, received, or draft message will be stored in SMS Storage, and User can acess them through the messaging menu, 
'Inbox', 'Outbox', or 'Draft'. 
User may erase a particular message or messages from particular category (Inbox/Outbox/Drafts) from the storage


This application also allows user to change the MO SMS encoding in sending a message,
which is available through 'Options' menu. 
The list of supported MO SMS encoding supported by a device is populated at application start up.

For SMS interface implementation details, please refer to the SMS How-To document.


--------------------
RUNTIME INSTRUCTIONS
--------------------
1. Start the application from BREW App Manager.
2. Select 'Compose message' to send a message
   a) Enter the destination number (currently it doesn't support sending to an international number).
   b) Move the cursor to text field to start typing your message.
   c) Media content (a small picture or a large picture) can be inserted to the message.
      If you insert a media content, the info will be populated at the 'inserted' and 'pos' field.
   d) After user hits 'send', you will get screen notification once the message has been sent.
   e) User will be able to access the sent message from 'Outbox'
3. You will get notification on incoming message, and access the received message on 'Inbox'.
4. If user exit the app while composing a message, it will be saved as a draft. 
5. User is also able to erase message(s) from particular category (Inbox/Outbox/Draft) 
   by going through 'Options' menu accessible from the application's Main menu
6. User may change the MO SMS encoding through the 'Options' Menu.
   **Note: When sending message across different network, user has to change the encoding 
   to AEESMS_ENC_GSM_7_BIT_DEFAULT (for network interoperability).