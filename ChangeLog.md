# ChangeLog

## 1.1.0 (2019-07-18)
 * Add specialized operation to create update contact poll message with additional recipients
   (sponsoring registrars of domains where changed contact is assigned as holder or admin-c)
    * Change appropriate implementations to use this new operation instead of previous one
      `CreatePollMessage<update_contact> -> CreateUpdateOperationPollMessage<contact>`

## 1.0.2 (2019-06-10)
 * Add move constructor into case insensitive comparator

## 1.0.1 (2019-05-10)
 * Add README with basic info about repo and versioning guide

## 1.0.0 (2019-03-20)
 * Initial release - library moved from `server` project to standalone repository
