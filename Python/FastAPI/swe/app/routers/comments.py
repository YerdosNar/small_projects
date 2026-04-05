from fastapi import APIRouter, HTTPException, Depends, status
from datetime import timedelta
from datetime import datetime
from uuid import UUID

from app.models import db, Comment
from app.schemas import CommentCreate, CommentOut
from app.dependencies import get_current_user
from app.routers.topics import purge_expired_topics, is_expired

router = APIRouter(prefix="/comments", tags=["Comments"])


# --- Leave a Comment ---

@router.post("/{topic_id}", response_model=CommentOut, status_code=status.HTTP_201_CREATED)
def create_comment(topic_id: str, payload: CommentCreate, current_user=Depends(get_current_user)):
    purge_expired_topics()

    try:
        uid = UUID(topic_id)
    except ValueError:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Invalid UUID")

    topic = db["topics"].get(uid)
    if not topic:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Topic not found")

    if is_expired(topic):
        raise HTTPException(status_code=status.HTTP_410_GONE, detail="Topic has expired")

    # Create comment
    new_comment = Comment(
        owner_id=current_user.id,
        topic_id=uid,
        content=payload.content
    )
    db["comments"][new_comment.id] = new_comment

    # Update topic TTL and counters
    topic.total_comment_number += 1
    topic.ttl += timedelta(minutes=1)

    return new_comment


# --- List Comments under a Topic ---

@router.get("/{topic_id}", response_model=list[CommentOut])
def list_comments(topic_id: str, current_user=Depends(get_current_user)):
    purge_expired_topics()

    try:
        uid = UUID(topic_id)
    except ValueError:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Invalid UUID")

    topic = db["topics"].get(uid)
    if not topic:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Topic not found")

    return [c for c in db["comments"].values() if c.topic_id == uid]
