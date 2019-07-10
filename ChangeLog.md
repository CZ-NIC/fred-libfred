# ChangeLog

## 3.0.0 (2019-06-10)
 * Fix operations for retrieving historical data (too many parameters in query)
 * Simplify logging interface

## 2.0.0 (2019-06-25)
 * Add new operations for retrieving historical data of domains and their state flags (by _id_, _fqdn_, _uuid_)
 * Add new operations for retrieving historical data of nssets and their state flags (by _id_, _handle_, _uuid_)
 * Add new operations for retrieving historical data of keysets and their state flags (by _id_, _handle_, _uuid_)
 * Add new operations for retrieving registration history of FQDNs, nsset and keyset handles
 * Add a new operation for manipulating registrar credit
     * Initialize registrar's credit account for a specific zone on the first credit transaction
 * Change the interface of the operation for updating registrar details
 * Replace the operation for retrieving registrar zone access with an operation to get all registrar zone access history
 * Fix exceptions of operations concerning registrars, registrar zone access and registrar certifications

## 1.0.2 (2019-06-10)
 * Add move constructor into case insensitive comparator

## 1.0.1 (2019-05-10)
 * Add README with basic info about repo and versioning guide

## 1.0.0 (2019-03-20)
 * Initial release - library moved from `server` project to standalone repository
