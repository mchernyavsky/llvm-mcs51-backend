from __future__ import annotations

import argparse
import json
import subprocess
import time
from dataclasses import dataclass

from scripts.common import ensure_tool


COPILOT_LOGIN = "copilot-pull-request-reviewer"

QUERY = """
query($owner:String!, $name:String!, $number:Int!) {
  repository(owner:$owner, name:$name) {
    pullRequest(number:$number) {
      number
      title
      url
      reviews(first:100) {
        nodes {
          author { login }
          state
          submittedAt
        }
      }
      reviewThreads(first:100) {
        nodes {
          id
          isResolved
          isOutdated
          path
          line
          comments(first:20) {
            nodes {
              author { login }
              body
              url
              createdAt
            }
          }
        }
      }
    }
  }
}
"""


@dataclass
class ReviewThread:
    path: str | None
    line: int | None
    body: str
    url: str
    author: str | None
    is_resolved: bool
    is_outdated: bool


def fetch_pr(repo: str, number: int) -> dict:
    ensure_tool("gh")
    owner, name = repo.split("/", 1)
    result = subprocess.run(
        [
            "gh",
            "api",
            "graphql",
            "-f",
            f"owner={owner}",
            "-f",
            f"name={name}",
            "-F",
            f"number={number}",
            "-f",
            f"query={QUERY}",
        ],
        text=True,
        check=True,
        capture_output=True,
    )
    return json.loads(result.stdout)["data"]["repository"]["pullRequest"]


def get_threads(pr: dict) -> list[ReviewThread]:
    threads: list[ReviewThread] = []
    for node in pr["reviewThreads"]["nodes"]:
        comments = node["comments"]["nodes"]
        last = comments[-1] if comments else None
        threads.append(
            ReviewThread(
                path=node["path"],
                line=node["line"],
                body=(last["body"] if last else "").strip(),
                url=(last["url"] if last else pr["url"]),
                author=(last["author"]["login"] if last and last["author"] else None),
                is_resolved=node["isResolved"],
                is_outdated=node["isOutdated"],
            )
        )
    return threads


def has_copilot_feedback(pr: dict) -> bool:
    for review in pr["reviews"]["nodes"]:
        author = review["author"]
        if author and author["login"] == COPILOT_LOGIN:
            return True

    for thread in get_threads(pr):
        if thread.author == COPILOT_LOGIN:
            return True

    return False


def print_unresolved_threads(threads: list[ReviewThread]) -> None:
    if not threads:
        return

    print("Unresolved review threads:")
    for thread in threads:
        location = f"{thread.path}:{thread.line}" if thread.path else "<unknown>"
        stale = " outdated" if thread.is_outdated else ""
        author = thread.author or "unknown"
        summary = thread.body.splitlines()[0] if thread.body else "<no body>"
        print(f"- {location}{stale} by {author}: {summary}")
        print(f"  {thread.url}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Fail if a PR still has unresolved review threads."
    )
    parser.add_argument("--repo", required=True, help="GitHub repo in owner/name form")
    parser.add_argument("--pr", type=int, required=True, help="Pull request number")
    parser.add_argument(
        "--require-copilot",
        action="store_true",
        help="Wait for Copilot review feedback before succeeding",
    )
    parser.add_argument(
        "--wait-seconds",
        type=int,
        default=600,
        help="How long to wait for Copilot feedback when --require-copilot is set",
    )
    parser.add_argument(
        "--poll-seconds",
        type=int,
        default=15,
        help="Polling interval used while waiting for Copilot feedback",
    )
    args = parser.parse_args()

    deadline = time.time() + max(args.wait_seconds, 0)
    saw_copilot = False

    while True:
        pr = fetch_pr(args.repo, args.pr)
        saw_copilot = saw_copilot or has_copilot_feedback(pr)
        unresolved = [thread for thread in get_threads(pr) if not thread.is_resolved]

        if args.require_copilot and not saw_copilot and time.time() < deadline:
            remaining = int(max(deadline - time.time(), 0))
            print(
                f"Waiting for Copilot review on PR #{args.pr} "
                f"({remaining}s remaining)..."
            )
            time.sleep(max(args.poll_seconds, 1))
            continue

        if args.require_copilot and not saw_copilot:
            raise SystemExit(
                f"Copilot review did not appear for PR #{args.pr} within "
                f"{args.wait_seconds}s"
            )

        if unresolved:
            print_unresolved_threads(unresolved)
            raise SystemExit(
                f"PR #{args.pr} still has {len(unresolved)} unresolved review thread(s)"
            )

        print(f"PR #{args.pr} is clear: no unresolved review threads remain.")
        return


if __name__ == "__main__":
    main()
