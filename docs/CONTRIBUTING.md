# Contributing to Vinum

Vinum is open source and you are welcome and encouraged to contribute.

These are a few guidelines.

## Contribution workflow

The official Vinum repository is at https://github.com/monacofj/Vinum
  
To start contributing, make sure you've read the essential documentation
thoroughly:

   - [vinum.md](/docs/vinum.md)
   - [CONTRIBUTING.md](/docs/CONTRIBUTING.md])
   - [CODE_OF_CONDUCT.md](/docs/CODE_OF_CONDUCT.md)

After that, follow these steps:

1. Create a local branch for the issue.
1. Edit the relevant files in the branch
1. Push your branch into your repository
1. Got to github and make it into a pull/merge request
1. Go have a cup of wine, as you’ve earned it

## Project standards

To keep things consistent, when applicable, we aim at some some standards.

- [REUSE specification vr. 3][reuse]

- [GitFlow branching strategy][git-flow]

- [Semantic versioning 2.0.0][semver]

## Code convention

Oh, that.

Code convention always tend to be a sensitive topic among programmers as it
calls forth personal convictions and the weight of beloved traditions. As often,
preferences vary largely. With that, we humbly as you to be indulgent about the
choices made in this project.

* Symbol names, comments, file names etc. always in English.

* Casing, indentation, block alignment etc.: stick to the project's conventions.

* Comments are text. Use caption and punctuation accordingly.


## Attribution and licensing

If you have substantially modified an existing source or documentation
file --- say it's considerably more than a simple typo correction or
small bug fix --- you are entitled to have your name added to the copyright
notice [1] and to the `AUTHORS` file, should you desire.

By submitting your contribution you agree that it will be available under the
same license as SYSeg (GNU GPL vr. 3 or later).

## Commit Messages

We really value commit messages, we think it's the way of keeping record of the
project. Following the convention, the commit messages should not exceed 72
columns of text width.

All the text should be on an imperative form, in a way that you're giving orders
to the project (e.g. "Fix spell error").

This is the commit message style we follow.

```gitcommit
[tag]: [Short description]

[Commit body]
```

### Tag

It's serves for stating the area of the project were the commit is from. Usually
is a path to a directory of file, if in doubt of what tag to use, use `git log
<file>` in one of the files that you're touching.

### Short description

It's meant to be a one-line summary of the changes. Do not try to describe
everything, you have less than 72 chars to work with. The commit body is for
that, just give a basic idea of what have been done.

Also, do not put a dot at the end of this line.

### Commit body

Here you explain the purpose of the commit: what's the problem, why it's needed
and how you solved it. Try to be very clear, as if you're describing to someone
that's doesn't knows about the issue you're solving. You can use markdown
notation if you want.

The text should be on a time tense where the changes are not yet present on the
project, so for example, if you're describing a bug on the code you should not
write it in this way:

```
The code had a bug, so we fixed it.
```

Instead, this is the proper way:

```
The code has a bug, so let's fix it.
```

If the commit solves an issue, please use a [closing keyword][closing-keyword]
on the commit body.

If you're finding it difficult to write commit messages in this format, maybe
you're cramming too much stuff into a single commit, try splitting it into
smaller commits that do just one thing.

For more information on commit message guidelines, please follow this
[blogpost](https://cbea.ms/git-commit/).

## PR/MR merging

Contributions should be done as a PR to the `develop` branch.

All PR's that change the code in any functional way must come with a test using
the VUnit framework.

## Other conventions

Compliance to Keep a [ChangeLog][changelog] is under consideration.


[reuse]: https://reuse.software

[git-flow]: https://nvie.com/posts/a-successful-git-branching-model/

[semver]: https://semver.org/

[closing-keyword]: https://docs.github.com/en/issues/tracking-your-work-with-issues/using-issues/linking-a-pull-request-to-an-issue#linking-a-pull-request-to-an-issue-using-a-keyword

[changelog]: https://keepachangelog.com/en/1.0.0/
