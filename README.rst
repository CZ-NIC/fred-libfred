.. image:: https://fred.nic.cz/documentation/html/_static/fred-logo.png
   :target: https://fred.nic.cz
   :alt: FRED

=============================
FRED: The Programming Library
=============================

..

   FRED basic entities operations library (C++)


LibFred is a library implementing basic operations (create/update/delete/info etc.)
on core registry objects (registrar, domain, contact, nsset, keyset, history records,
and related).

This repository is a subproject of FRED, the Free Registry for ENUM and Domains,
and it contains only a fraction of the source code required for running FRED.
See the
`complete list of subprojects <https://fred.nic.cz/documentation/html/Architecture/SourceCode.html>`_
that make up FRED.

Learn more about the project and our community on the `FRED's home page <https://fred.nic.cz>`_.

Documentation for the whole FRED project is available on-line, visit https://fred.nic.cz/documentation.

Table of Contents
=================


* `Dependencies <#dependencies>`_
* `Configuration <#configuration>`_
* `Development <#development>`_

  * `Building <#building>`_
  * `Versioning <#versioning>`_

* `Maintainers <#maintainers>`_
* `License <#license>`_

Dependencies
============

Dependencies are managed with CMake and they are listed in a docker image
for each supported platform in `scripts/docker <scripts/docker>`_.

Configuration
=============

See the example configuration in `libfred.conf.example <libfred.conf.example>`_.

Development
===========

Building
--------

This component is not to be built separately, but as a part of ``fred-server``.

Versioning
----------

Use semantic versioning ``major.minor.patch`` and bump:


* ``major`` version when you make incompatible API changes,
* ``minor`` version when you add functionality in a backwards-compatible manner, and
* ``patch`` version when you make backwards-compatible bug fixes.

For pre-releases use ``-rcX`` suffix (\ ``major.minor.patch-rcX``\ )
where ``X`` starts at ``1`` and consecutively increments.

Maintainers
===========


* Jiri Sadek [jiri.sadek@nic.cz](mailto:jiri.sadek@nic.cz)
* Jaromir Talir [jaromir.talir@nic.cz](mailto:jaromir.talir@nic.cz)

License
=======

See `LICENSE <LICENSE>`_.
