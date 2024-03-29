ChangeLog
=========

7.0.0 (2022-11-21)
------------------

* Rework interfaces and implementation of operations manipulating registrars (mainly to better reflect actually mandatory attributes)

  * `CreateRegistrar`, `UpdateRegistrarById`
  * Add new exceptions


6.1.1 (2022-09-21)
------------------

* Merge fixes from 5.3 branch

    * Fix ``*RegistrarEppAuth`` operations - optional ``create_time``


6.1.0 (2022-09-19)
------------------

* Merge new features from 5.3 branch

    * Create new object authinfo storage with hashed password and ttl (expiration timestamp)
    * Modify info / create/update / transfer operations of domain / contact / nsset / keyset to support authinfo with ttl


6.0.0 (2021-12-19)
------------------

* Update CI
* Update CMake build
* Use ``liblog`` for logging
* Add support to use ``libfred`` operations with ``libpg`` driven source code
  (allow to create ``libfred::OperationContext`` from ``libpg::*Transaction`` classes)


5.3.1 (2022-09-21)
------------------

* Fix ``*RegistrarEppAuth`` operations - optional ``create_time``


5.3.0 (2022-09-14)
------------------

* Create new object authinfo storage with hashed password and ttl (expiration timestamp)
* Modify info / create/update / transfer operations of domain / contact / nsset / keyset to support authinfo with ttl
* Update ``*RegistrarEppAuth`` operations with new attributes (``create_time``, ``uuid``, ``cert_data_pem``)


5.2.0 (2022-02-28)
------------------

* Normalize string data of contact (empty string / null values / whitespace) when finding duplicates and in merge operation
* Rework object factories to manual registration instead of auto-registration which was not working properly


5.1.0 (2022-01-25)
------------------

* Add possibility to mark registrar as internal
* Update CMake build
* Fix build warnings
* Fix tests


5.0.1 (2022-01-10)
------------------

* Change minimal version for PostgreSQL to 13 (CI image, build)


5.0.0 (2021-11-05)
------------------

* Add new contact states to inform that some of the contact attributes are locked and cannot be changed
* Modify *contact merge* operation and *contact auto merge procedure* to be aware of linked external identity to contact
* Modify *contact delete* operation to unlink external identity from contact


4.3.2 (2021-08-09)
------------------

* Rewrite query for update contact poll message (additional recipients) without using ``TSRANGE``


4.3.1 (2021-05-07)
------------------

* Fix build (install, distcheck)


4.3.0 (2021-04-30)
------------------

* Add bumpversion configuration
* Rename changelog to CHANGELOG.rst to match all FRED projects
* Fix CMake (distcheck, versioning)


4.2.2 (2021-08-09)
------------------

* Rewrite query for update contact poll message (additional recipients) without using ``TSRANGE``


4.2.1 (2021-04-21)
------------------

* Optimize registration check for enum domains


4.2.0 (2020-07-27)
------------------

* Add ``COPY FROM`` support to database layer
* Add new tests
* Fix CMake targets to simplify usage in dependent projects


4.1.2 (2021-08-09)
------------------

* Rewrite query for update contact poll message (additional recipients) without using ``TSRANGE``


4.1.1 (2021-04-21)
------------------

* Optimize registration check for enum domains


4.1.0 (2020-02-19)
------------------

* Provide ``libfred_LIBRARIES`` and ``libfred_INCLUDE_DIRS`` for dependent projects in CMake
* Remove unused shared library from build
* Fix warning messages


4.0.3 (2021-08-09)
------------------

* Rewrite query for update contact poll message (additional recipients) without using ``TSRANGE``


4.0.2 (2021-04-21)
------------------

* Optimize registration check for enum domains


4.0.1 (2019-11-18)
------------------

* Fix random sequence generator
* Fix CMake ``libpq`` detection (set ``PostgreSQL_ADDITIONAL_VERSIONS`` instead of ``PostgreSQL_KNOWN_VERSIONS``\ )
* Less strict locking of ``registrar_credit`` on new record initialization


4.0.0 (2019-09-11)
------------------

* Reworked random data generator interface (previous interfaces were removed)
* Fix overflow in ``SqlConvert<std::chrono::time_point<std::chrono::system_clock, R>>``


3.0.0 (2019-06-10)
------------------

* Fix operations for retrieving historical data (too many parameters in query)
* Simplify logging interface


2.0.0 (2019-06-25)
------------------

* Add new operations for retrieving historical data of domains and their state flags (by *id*\ , *fqdn*\ , *uuid*\ )
* Add new operations for retrieving historical data of nssets and their state flags (by *id*\ , *handle*\ , *uuid*\ )
* Add new operations for retrieving historical data of keysets and their state flags (by *id*\ , *handle*\ , *uuid*\ )
* Add new operations for retrieving registration history of FQDNs, nsset and keyset handles
* Add a new operation for manipulating registrar credit

  * Initialize registrar's credit account for a specific zone on the first credit transaction

* Change the interface of the operation for updating registrar details
* Replace the operation for retrieving registrar zone access with an operation to get all registrar zone access history
* Fix exceptions of operations concerning registrars, registrar zone access and registrar certifications


1.1.0 (2019-07-18)
------------------

* Add specialized operation to create update contact poll message with additional recipients
  (sponsoring registrars of domains where changed contact is assigned as holder or admin-c)

  * Change appropriate implementations to use this new operation instead of previous one
    ``CreatePollMessage<update_contact> -> CreateUpdateOperationPollMessage<contact>``


1.0.2 (2019-06-10)
------------------

* Add move constructor into case insensitive comparator


1.0.1 (2019-05-10)
------------------

* Add README with basic info about repo and versioning guide


1.0.0 (2019-03-20)
------------------

* Initial release - library moved from ``server`` project to standalone repository
