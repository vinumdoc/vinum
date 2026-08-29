=====================
Contributing to Vinum
=====================

Vinum is open source and you are welcome and encouraged to contribute.

These are a few guidelines.

Contribution workflow
---------------------

The official Vinum repository is at https://github.com/vinumdoc/vinum

1. Fork the project.
2. Create a local branch for the issue.
3. Edit the relevant files in the branch.
4. Add tests if necessary.
5. Build and test following the instruction on the docs/source/build.rst
6. Push your branch into your repository.
7. Go to GitHub and make it into a pull/merge request.
8. Go have a cup of wine, as you’ve earned it.

Project standards
-----------------

To keep things consistent, when applicable, we aim at some standards:

* `REUSE specification vr. 3 <https://reuse.software>`_
* `GitFlow branching strategy <https://nvie.com/posts/a-successful-git-branching-model/>`_
* `Semantic versioning 2.0.0 <https://semver.org/>`_

Code convention
---------------

Oh, that.

Code convention always tends to be a sensitive topic among programmers as it
calls forth personal convictions and the weight of beloved traditions. As often,
preferences vary largely. With that, we humbly ask you to be indulgent about the
choices made in this project.

* Symbol names, comments, file names etc. always in English.
* Casing, indentation, block alignment etc.: stick to the project's conventions.
* Comments are text. Use capitalization and punctuation accordingly.

This project uses ``clang-format`` for ensuring all code is equally formatted.

You can run ``meson compile --ninja-args=clang-format`` on your build directory
to format your changes.

Attribution and licensing
-------------------------

If you have substantially modified an existing source or documentation
file --- say it's considerably more than a simple typo correction or
small bug fix --- you are entitled to have your name added to the copyright
notice and to the ``AUTHORS`` file, should you desire.

By submitting your contribution you agree that it will be available under the
same license as SYSeg (GNU GPL vr. 3 or later).

Commit Messages
---------------

We really value commit messages; we think it's the way of keeping record of the
project. Following the convention, the commit messages should not exceed 72
columns of text width.

All the text should be on an imperative form, in a way that you're giving orders
to the project (e.g. "Fix spell error").

This is the commit message style we follow:

.. code-block::

   [tag]: [Short description]

   [Commit body]

Tag
^^^

It serves for stating the area of the project where the commit is from. Usually
is a path to a directory or file. If in doubt of what tag to use, use
``git log <file>`` on one of the files that you're touching.

Short description
^^^^^^^^^^^^^^^^^

It's meant to be a one-line summary of the changes. Do not try to describe
everything; you have less than 72 chars to work with. The commit body is for
that; just give a basic idea of what has been done.

Also, do not put a dot at the end of this line.

Commit body
^^^^^^^^^^^

Here you explain the purpose of the commit: what's the problem, why it's needed
and how you solved it. Try to be very clear, as if you're describing to someone
that doesn't know about the issue you're solving. You can use markdown
notation if you want.

The text should be on a time tense where the changes are not yet present on the
project. For example, if you're describing a bug on the code you should **not**
write it this way:

.. code-block:: text

   The code had a bug, so we fixed it.

Instead, this is the proper way:

.. code-block:: text

   The code has a bug, so let's fix it.

If the commit solves an issue, please use a `closing keyword <https://docs.github.com/en/issues/tracking-your-work-with-issues/using-issues/linking-a-pull-request-to-an-issue#linking-a-pull-request-to-an-issue-using-a-keyword>`_
on the commit body.

If you're finding it difficult to write commit messages in this format, maybe
you're cramming too much stuff into a single commit; try splitting it into
smaller commits that do just one thing.

For more information on commit message guidelines, please follow this
`blogpost <https://cbea.ms/git-commit/>`_.

PR/MR merging
-------------

Contributions should be done as a PR to the ``develop`` branch.

All PR's that change the code in any functional way must come with a test using
the VUnit framework.

Other conventions
-----------------

Compliance to `Keep a ChangeLog <https://keepachangelog.com/en/1.0.0/>`_ is
under consideration.
